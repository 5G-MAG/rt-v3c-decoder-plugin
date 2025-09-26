/*
* Copyright (c) 2025 InterDigital CE Patent Holdings SASU
* Licensed under the License terms of 5GMAG software (the "License").
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at https://www.5g-mag.com/license .
* Unless required by applicable law or agreed to in writing, software distributed under the License is
* distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and limitations under the License.
*/

#include <client/meta.h>
#include <client/reader.h>
#include <iloj/misc/json.h>
#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
#include <streaming/network_interface.h>
#endif // STREAMING
#include <iloj/misc/filesystem.h>

using namespace iloj::misc;

auto MetaInterface::getMediaList() -> const std::vector<std::string> &
{
#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
    return m_realInterface[1]->getMediaList();
#else
    return m_realInterface->getMediaList();
#endif
}

int MetaInterface::getMediaId() const 
{ 
#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
    return m_realInterface[1]->getMediaId();
#else
    return m_realInterface->getMediaId();
#endif
}

void MetaInterface::onConfigure(const std::string &configFile)
{
    auto json = JSON::Object::fromFile(configFile);

#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
    LOG_INFO("MetaInterface::onConfigure in remote mode");
    m_realInterface[0] = std::make_unique<NetworkInterface>();
    LOG_INFO("MetaInterface::onConfigure in local mode");
    m_realInterface[1] = std::make_unique<ReaderInterface>();
#else
    LOG_INFO("MetaInterface::onConfigure in local mode");
    m_realInterface = std::make_unique<ReaderInterface>();
#endif

    const auto libraryPath = FileSystem::Path::getAbsolute(
        {json.getItem<JSON::String>("Library").getValue(), FileSystem::Path{configFile}.getParent()});
    if (!FileSystem::File{libraryPath}.exist())
    {
        LOG_ERROR("Library file not found: ", libraryPath.toString());
        return;
    }

    auto jsonLibrary = JSON::Object::fromFile(libraryPath.toString());
    if (jsonLibrary.isEmpty())
    {
        LOG_ERROR("Library is unreadable");
        return;
    }

    const auto &jsonPlaylist = jsonLibrary.getItem<JSON::Array>("Playlist");
    const auto nbChannel = jsonPlaylist.getSize();
    if (!nbChannel)
    {
        LOG_ERROR("Playlist data is either missing or empty");
        return;
    }

    m_itemList = Item::makeItemList(jsonLibrary, nbChannel, true);

    if (m_itemList.empty())
    {
        LOG_ERROR("Playlist is empty or unreadable");
        return;
    }

#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
    // mode is remote
    m_realInterface[0]->setDecoderInterface(m_decoderInterface);
    m_realInterface[0]->onConfigure(configFile);

    // mode is local
    m_realInterface[1]->setDecoderInterface(m_decoderInterface);
    m_realInterface[1]->onConfigure(configFile);
#else
    m_realInterface->setDecoderInterface(m_decoderInterface);
    m_realInterface->onConfigure(configFile);
#endif
}

void MetaInterface::onStartEvent(unsigned mediaId) 
{ 
#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
    if ( (m_itemList[mediaId].getMode().compare("dash") == 0)
        || (m_itemList[mediaId].getMode().compare("rtp") == 0)
        || (m_itemList[mediaId].getMode().compare("webrtc") == 0)
        )
    {
        m_realInterface[0]->onStartEvent(mediaId);
    }
    else
    {
        m_realInterface[1]->onStartEvent(mediaId);
    }
#else
    m_realInterface->onStartEvent(mediaId);
#endif
}

void MetaInterface::onMediaRequest(unsigned mediaId)
{
#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
    if ((m_itemList[mediaId].getMode().compare("dash") == 0)
        || (m_itemList[mediaId].getMode().compare("rtp") == 0)
        || (m_itemList[mediaId].getMode().compare("webrtc") == 0)
        )
    {
        m_realInterface[0]->onMediaRequest(mediaId);
    }
    else
    {
        m_realInterface[1]->onMediaRequest(mediaId);
    }
#else
    m_realInterface->onMediaRequest(mediaId);
#endif
}

void MetaInterface::onStopEvent()
{
#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
    m_realInterface[0]->onStopEvent();
    m_realInterface[1]->onStopEvent();
#else
    m_realInterface->onStopEvent();
#endif
}
