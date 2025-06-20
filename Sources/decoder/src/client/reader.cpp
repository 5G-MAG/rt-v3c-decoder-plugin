/*
* Copyright (c) 2024 InterDigital R&D France
* Licensed under the License terms of 5GMAG software (the "License").
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at https://www.5g-mag.com/license .
* Unless required by applicable law or agreed to in writing, software distributed under the License is
* distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and limitations under the License.
*/

#include <client/reader.h>
#include <iloj/misc/filesystem.h>

using namespace iloj::misc;

void ReaderInterface::onConfigure(const std::string &configFile)
{
    LOG_INFO("ReaderInterface::onConfigure ", configFile);

    if (!FileSystem::File{configFile}.exist())
    {
        LOG_ERROR("Configuration file not found: ", configFile);
    }

    auto dataDirectory = FileSystem::Path{configFile}.getParent();
    auto jsonPath = FileSystem::Path::getAbsolute(
        {JSON::Object::fromFile(configFile).getItem<JSON::String>("Library").getValue(), dataDirectory});

    if (!FileSystem::File{jsonPath}.exist())
    {
        LOG_ERROR("Library file not found: ", jsonPath.toString());
    }

    auto config = JSON::Object::fromFile(jsonPath.toString());

    config.setItem<JSON::String>("BaseDirectory", jsonPath.getParent().toString());

    const auto &playList = config.getItem<JSON::Array>("Playlist");

    auto nbChannel = playList.getSize();

    m_itemList.clear();

    for (std::size_t channelId = 0; channelId < nbChannel; channelId++)
    {
        m_itemList.emplace_back(config, channelId, true);
        m_mediaList.emplace_back(m_itemList.back().getName());
    }

    config.toFile(jsonPath.toString());
}

void ReaderInterface::onStartEvent(unsigned mediaId)
{
    LOG_INFO("ReaderInterface::onStartEvent ");
    m_requestedItemId = mediaId;
    start();
}

void ReaderInterface::onMediaRequest(unsigned mediaId)
{
    m_requestedItemId = mediaId;
    LOG_INFO("ReaderInterface: Channel request successfully set to ", mediaId);
}

void ReaderInterface::onStopEvent()
{
    stop();

    LOG_INFO("ReaderInterface::onStopEvent");
}

void ReaderInterface::initialize()
{
    m_t0 = m_timer.restart();

    m_currentItemId = m_itemList.size();
}

void ReaderInterface::idle()
{
    try
    {
        updateItem();
    }
    catch (std::exception e)
    {
        LOG_ERROR(e.what());
        throw;
    }
}

void ReaderInterface::updateItem()
{
    // Change item if necessary
    if (m_requestedItemId != m_currentItemId)
    {
        m_currentItemId = m_requestedItemId;

        m_itemList[m_currentItemId].reset();

        m_delay.resize(m_itemList[m_currentItemId].getNumberOfStreams());
        std::fill(m_delay.begin(), m_delay.end(), std::chrono::duration<double>{});

        m_t0 = m_timer.restart();

        m_checkpoint = std::chrono::duration<double>::zero();
        m_stop = false;
    }

    if (m_stop)
    {
        return;
    }

    // check if delay is reached
    if (std::chrono::system_clock::now().time_since_epoch() < m_checkpoint)
    {
        return;
    }

    // Send chunk
    auto [streamId, chunck, duration] = m_itemList[m_currentItemId].next();

    if (chunck.getData().empty())
    {
        if (m_decoderInterface != nullptr)
        {
            auto cb = m_decoderInterface->getOnErrorEventCallback();
            if (cb != nullptr)
            {
                cb(2, (unsigned int)LocalCode::LOCAL_INVALID_FILE);
            }
            m_stop = true;
        }
        return;
    }

    if (!m_loop_stream && chunck.getHeader().getSegmentId() == m_itemList[m_currentItemId].getStreams()[streamId].getNumberOfSegments()-1)
    {
        m_stop = true;
    }

    if ((streamId == 0) && (chunck.getHeader().getSegmentId() == 0))
    {
        std::fill(m_delay.begin(), m_delay.end(), m_delay[0]);
    }

    m_checkpoint = m_t0.time_since_epoch() + m_delay[streamId]; // at first iteration, m_delay[streamId] == 0. the next
                                                                // segment will be bufferised without delay
    auto pts = m_checkpoint + m_lookAhead;
    m_delay[streamId] += duration;

    chunck.getHeader().setPTS(pts);
    chunck.getHeader().setDuration(duration);

    // Pushing packet
    if (m_decoderInterface)
    {
        m_decoderInterface->onChunkEvent(std::move(chunck));
    }
}

void ReaderInterface::finalize() { LOG_INFO("ReaderInterface::finalize"); }
