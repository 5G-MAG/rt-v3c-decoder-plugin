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

#include <common/decoder/miv.h>
#include <common/decoder/vpcc.h>
#include <common/stream/item.h>
#include <iloj/media/avcodec.h>
#include <iloj/misc/filesystem.h>

using namespace iloj::misc;
using namespace iloj::media;

namespace
{
auto getTypeFromString(const std::string &name) -> std::uint8_t
{
    if (name == "audio")
    {
        return Chunk::Header::TypeId::Audio;
    }
    else if (name == "hevc")
    {
        return Chunk::Header::TypeId::Hevc;
    }
    else if (name == "vvc")
    {
        return Chunk::Header::TypeId::Vvc;
    }
    else if (name == "miv")
    {
        return Chunk::Header::TypeId::Miv;
    }
    else if (name == "vpcc")
    {
        return Chunk::Header::TypeId::Vpcc;
    }
    else if (name == "haptic")
    {
        return Chunk::Header::TypeId::Haptic;
    }

    return Chunk::Header::TypeId::None;
}

auto getSegmentProperty(int typeId, const std::string &path) -> std::pair<double, std::uint32_t>
{
    switch (typeId)
    {
        case Chunk::Header::TypeId::Audio:
        {
            AVCodec::Decoder decoder;
            decoder.open(path, { AVCodec::Decoder::Stream::BestAudio});
            auto info = decoder.getInformation().getItem<JSON::Array>("Streams").getItem<JSON::Object>(0);
            return {info.getItem("Duration").as<double>(), info.getItem("NbFrame").as<std::uint32_t>()};
        }
        case Chunk::Header::TypeId::Hevc:
        case Chunk::Header::TypeId::Vvc:
        {
            AVCodec::Decoder decoder;
            decoder.open(path, { AVCodec::Decoder::Stream::BestVideo});
            auto info = decoder.getInformation().getItem<JSON::Array>("Streams").getItem<JSON::Object>(0);
            return {info.getItem("Duration").as<double>(), info.getItem("NbFrame").as<std::uint32_t>()};
        }
        case Chunk::Header::TypeId::Miv:
        {
            AVCodec::Decoder decoder;
            auto inputData = FileSystem::File{path}.toBuffer();

            auto [mivPkt, videoDataPacketList] =
                miv::decodeMivBuffer({reinterpret_cast<const char *>(inputData.data()), inputData.size()});
            auto frameRate = static_cast<double>(mivPkt.vui->vui_time_scale()) / mivPkt.vui->vui_num_units_in_tick();

            decoder.getStreamingInput().push(videoDataPacketList[VideoStream::Texture]);
            decoder.getStreamingInput().push(Packet<Descriptor::Data>{});

            decoder.open("", { AVCodec::Decoder::Stream::BestVideo});
            auto info = decoder.getInformation().getItem<JSON::Array>("Streams").getItem<JSON::Object>(0);
            auto nbFrame = info.getItem("NbFrame").as<std::uint32_t>();

            return {nbFrame / frameRate, nbFrame};
        }
        case Chunk::Header::TypeId::Vpcc:
        {
            AVCodec::Decoder decoder;
            auto inputData = FileSystem::File{path}.toBuffer();

            auto [mivPkt, videoDataPacketList] = decodeVpccBuffer(inputData);

            auto frameRate = 30.0;
            // static_cast<double>(mivPkt->vui->vui_time_scale()) / mivPkt->vui->vui_num_units_in_tick();

            decoder.getStreamingInput().push(videoDataPacketList[VideoStream::Texture]);
            decoder.getStreamingInput().push(Packet<Descriptor::Data>{});

            decoder.open("", { AVCodec::Decoder::Stream::BestVideo});
            auto info = decoder.getInformation().getItem<JSON::Array>("Streams").getItem<JSON::Object>(0);
            auto nbFrame = info.getItem("NbFrame").as<std::uint32_t>();

            return {nbFrame / frameRate, nbFrame};
        }
        case Chunk::Header::TypeId::Haptic:
        {
            // Irrelevant
            return {1.0f, 32};
        }
        default:
            return {};
    }
}

} // namespace

Item::Item(JSON::Object &config, int itemId, bool buildIndex)
{
    auto &jsonDirectory = config.getItem<JSON::String>("BaseDirectory").getValue();
    auto &jsonItem = config.getItem<JSON::Array>("Playlist").getItem<JSON::Object>(itemId);
    auto &baseDirectory = jsonItem.getItem<JSON::String>("BaseDirectory").getValue();
    auto &streamList = jsonItem.getItem<JSON::Array>("StreamList");
   
    m_itemId = itemId;

    if (jsonItem.hasItem("Name"))
    {
        m_name = jsonItem.getItem<JSON::String>("Name").getValue();
    }
    else
    {
        m_name = "NONAME_" + int2str(m_itemId, 3);
    }
    if (jsonItem.hasItem("Mode"))
    {
        m_mode = jsonItem.getItem<JSON::String>("Mode").getValue();
    }
    else
    {
        m_mode = "Local";
    }
    

    for (auto streamId = 0ULL; streamId < streamList.getSize(); streamId++)
    {
        auto &stream = streamList.getItem<JSON::Object>(streamId);

        m_streamList.emplace_back(getTypeFromString(stream.getItem<JSON::String>("Type").getValue()),
                                  stream.getItem("NbSegment").as<int>(),
                                  FileSystem::Path::getAbsolute(
                                      {stream.getItem<JSON::String>("Path").getValue(), baseDirectory, jsonDirectory})
                                      .toString(),
                                  stream.getItem<JSON::String>("Url").getValue(),
                                  stream.getItem<JSON::String>("ServerName").getValue(),
                                  //stream.getItem<JSON::Object>("Decoder").getItem("Thread").as<unsigned>(),
                                  //stream.getItem<JSON::Object>("Decoder").getItem("HardwareAcceleration").as<bool>(),
                                  stream.getItem<JSON::String>("SenderName").getValue(),
                                  stream.hasItem("Framerate") ? stream.getItem("Framerate").as<double>() : 25.0
                                  );

        m_streamState.emplace_back(0, 0.);
    }

    std::vector<std::size_t> streamOrder(m_streamList.size());
    std::iota(streamOrder.begin(), streamOrder.end(), 0ULL);

    std::sort(streamOrder.begin(),
              streamOrder.end(),
              [this](auto i1, auto i2) { return (m_streamList[i1].getTypeId() <= m_streamList[i2].getTypeId()); });

    std::sort(m_streamList.begin(),
              m_streamList.end(),
              [](const auto &s1, const auto &s2) { return (s1.getTypeId() <= s2.getTypeId()); });

    m_streamProperty.resize(m_streamList.size());

    if (buildIndex)
    {
        // in remote mode (DASH), Duration and NbFrame have no meaning, so skip them.
        if (!to_lower(m_mode).compare("local"))
        {
            for (std::size_t streamId = 0ULL; streamId < m_streamList.size(); streamId++)
            {
                auto &stream = streamList.getItem<JSON::Object>(streamOrder[streamId]);

                if (stream.hasItem("Duration") && stream.hasItem("NbFrame"))
                {
                    auto &duration = stream.getItem<JSON::Array>("Duration");
                    auto &nbFrame = stream.getItem<JSON::Array>("NbFrame");

                    for (auto segmentId = 0; segmentId < m_streamList[streamId].getNumberOfSegments(); segmentId++)
                    {
                        m_streamProperty[streamId].emplace_back(std::make_pair(
                            duration.getItem(segmentId).as<double>(), nbFrame.getItem(segmentId).as<std::uint32_t>()));
                    }
                }
                else
                {
                    auto &duration = stream.setItem<JSON::Array>("Duration");
                    auto &nbFrame = stream.setItem<JSON::Array>("NbFrame");

                    const auto &streamElement = m_streamList[streamId];

                    for (auto segmentId = 0; segmentId < m_streamList[streamId].getNumberOfSegments(); segmentId++)
                    {
                        auto path = format(streamElement.getPath().c_str(), segmentId);
                        auto property = getSegmentProperty(streamElement.getTypeId(), path);

                        duration.setItem<JSON::Float>(segmentId, static_cast<float>(property.first));
                        nbFrame.setItem<JSON::Integer>(segmentId, static_cast<int>(property.second));

                        m_streamProperty[streamId].emplace_back(property);
                    }
                }
            }
        }
    }
}

void Item::reset() { std::fill(m_streamState.begin(), m_streamState.end(), State{0, 0.}); }

auto Item::next() -> std::tuple<std::size_t, Chunk, std::chrono::duration<double>>
{
    // Finding best stream to send (the most late)
    auto iter =
        std::min_element(m_streamState.begin(),
                         m_streamState.end(),
                         [](const auto &s1, const auto &s2) { return (s1.getStreamDelay() < s2.getStreamDelay()); });

    if (iter->getSegmentId() < 0)
    {
        std::fill(m_streamState.begin(), m_streamState.end(), State{0, 0.});
        iter = m_streamState.begin();
    }

    auto bestStreamId = static_cast<std::size_t>(std::distance(m_streamState.begin(), iter));

    const auto &stream = m_streamList[bestStreamId];
    auto &property = m_streamProperty[bestStreamId];
    auto &state = m_streamState[bestStreamId];

    // Preparing chunk
    auto path = format(stream.getPath().c_str(), state.getSegmentId());
    auto data = FileSystem::File{path}.toBuffer();

    if (data.empty())
    {
        LOG_ERROR("Invalid stream file: ", path);
    }

    if (static_cast<int>(property.size()) <= state.getSegmentId())
    {
        property.emplace_back(getSegmentProperty(stream.getTypeId(), path));
    }

    const auto &[duration, nbFrame] = property[state.getSegmentId()];

    // Filling chunk
    Chunk::Header header;

    header.setTypeId(stream.getTypeId());
    header.setMediaId(m_itemId);
    header.setSegmentId(state.getSegmentId());
    header.setNumberOfFrames(nbFrame);
    header.setDataSize(data.size());

    // Updating state    
    state.update(duration, stream.getNumberOfSegments());

    return {bestStreamId, {header, std::move(data)}, std::chrono::duration<double>{duration}};
}

std::vector<Item> Item::makeItemList(JSON::Object &config, unsigned int nbChannel, bool buildIndex)
{
    std::vector<Item> itemList;

    for (std::size_t channelId = 0; channelId < nbChannel; channelId++)
    {
        itemList.emplace_back(config, channelId, buildIndex);
    }

    return itemList;
}
