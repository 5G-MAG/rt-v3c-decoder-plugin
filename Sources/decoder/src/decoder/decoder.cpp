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

#include <common/decoder/miv.h>
#include <common/decoder/vpcc.h>
#include <decoder/decoder.h>
#include <iloj/gpu/framework/native/processor.h>
#include <iloj/misc/dll.h>
#include <iloj/misc/filesystem.h>
#include <iomanip>

#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
#include <streaming/network_interface.h>
#endif // STREAMING

#ifdef __ANDROID__
#include <iloj/misc/jni.h>
#endif

using namespace std;
using namespace iloj::misc;
using namespace iloj::media;
using namespace iloj::gpu;

std::array<std::unique_ptr<Processor>, VideoStream::Size> DecoderInterface::g_procVideoDecodingList;

#ifdef __ANDROID__
extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void * /* reserved */)
{
    JNI::Context::getInstance().setJavaVirtualMachine(vm);
    return JNI_VERSION_1_6;
}
#endif

DecoderInterface::~DecoderInterface() { LOG_INFO("DecoderInterface::~DecoderInterface"); }

void DecoderInterface::onConfigure(const std::string &configFile)
{
    LOG_INFO("DecoderInterface::onConfigure ", configFile);

    auto json = JSON::Object::fromFile(configFile);

    m_glInteroperability = json.getItem<JSON::Object>("Decoder").getItem("OpenGLInteroperability").as<bool>();

    if (auto &item = json.getItem<JSON::Object>("Decoder").getItem("MeasureFPS"))
    {
        m_measureFPS = item.as<bool>();
    }

    auto &jsonConfigList = json.getItem<JSON::Object>("Decoder").getItem<JSON::Array>("ConfigList");
    const auto nbConfig = jsonConfigList.getSize();

    for (size_t itemId = 0; itemId < nbConfig; itemId++)
    {
        auto &jsonItem = jsonConfigList.getItem<JSON::Object>(itemId);

        const std::string name = jsonItem.getItem<JSON::String>("Name").getValue();
        unsigned thNb = jsonItem.getItem("Thread").as<unsigned>();
        bool hwAcc = jsonItem.getItem("HardwareAcceleration").as<bool>();
        std::string androidFormat = jsonItem.getItem<JSON::String>("AndroidFormat").getValue();
        Config conf = {name, thNb, hwAcc, androidFormat};
        m_configMap.insert(std::pair(name, conf));
    }
    
    m_avcodec_name = json.getItem<JSON::Object>("Decoder").getItem<JSON::String>("AVCodec").getValue();
    if (m_avcodec_name.empty())
    {
        LOG_ERROR("AVCodec file undefined");
    }

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
}

void DecoderInterface::setSharedOpenGLContext(HANDLE hwContext)
{
    LOG_INFO("DecoderInterface::setSharedOpenGLContext");

    if (m_glInteroperability && !g_procVideoDecodingList.front())
    {
        for (auto &procVideoDecoding : g_procVideoDecodingList)
        {
#ifdef _WIN64
            procVideoDecoding =
                std::make_unique<framework::native::Processor>(reinterpret_cast<HGLRC>(hwContext), true);
#elif defined __ANDROID__
            procVideoDecoding =
                std::make_unique<framework::native::Processor>(reinterpret_cast<EGLContext>(hwContext), true);
#else
            // TODO
#endif
        }

        LOG_INFO("GL decoding processors allocated");
    }
}

void DecoderInterface::onStartEvent(unsigned mediaId)
{
    LOG_INFO("DecoderInterface::onStartEvent");

    std::lock_guard<SpinLock> guard(m_locker);
    m_Tpkt = std::chrono::high_resolution_clock::now();

#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
    if (( m_itemList[mediaId].getMode().compare("dash") == 0)
        || (m_itemList[mediaId].getMode().compare("rtp") == 0)
        || (m_itemList[mediaId].getMode().compare("webrtc") == 0)
        )
    {
        m_streamingMode = true;
    }
    else
    {
        m_streamingMode = false;
    }
#endif // STREAMING

    allocateHapticDecoder();
    allocateAudioDecoder(m_avcodec_name);
    allocateVideoDecoders(m_avcodec_name);

#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
    if (m_streamingMode)
    {
        m_streamingFirstFrameAudio = true;
        m_streamingFirstPtsVideo = true;
    }
#endif // STREAMING

    m_hapticInitTime = std::chrono::duration<double>(0);

    m_requestedItemId = mediaId;
    start();
}


void DecoderInterface::onStopEvent()
{
    
     std::lock_guard<SpinLock> guard(m_locker);

    //close inputs before calling stop.
    //when calling stop, onStop, idle and finalize are concurrent, so closing
    //the inputs in onStop can make them close too late.
    m_genericInput.close();

    for (auto &videoInput : m_videoInputList)
        videoInput.close();

    stop();

    m_audioDecoder.reset();

    for (auto &videoDecoder : m_videoDecoderList)
    {
        videoDecoder.reset();
    }

    LOG_INFO("DecoderInterface::onStopEvent");
}


void DecoderInterface::onChunkEvent(Chunk &&chunk)
{
    std::lock_guard<SpinLock> guard(m_locker);

    if (chunk.getHeader().getMediaId() == m_requestedItemId)
    {
        if (m_avcodec_name.empty())
        {
            auto cb = getOnErrorEventCallback();
            if (cb != nullptr)
            {
                cb(2 /* Error */, 201);
            }
            return;
        }

        auto pkt = make_packet<Chunk>(std::move(chunk));

        switch (pkt->getHeader().getTypeId())
        {
            case Chunk::Header::TypeId::Audio:
            {
                if (m_audioDecoder)
                {
                    for (std::uint32_t frameId = 0; frameId < pkt->getHeader().getNumberOfFrames(); frameId++)
                    {
                        //LOG_INFO("AUDIO: onChunkEvent push chunk in audioChunkQueue");
                        // in case of DASH, a frame is an m4s segment
                        m_audioChunkQueue.push(pkt);
                    }
                    // push data in the streaming queue. to be decoded by AVCodec decoder
                    m_audioDecoder->getStreamingInput().push(make_packet<Descriptor::Data>(std::move(pkt->getData())));

                    if (!m_audioDecoder->is_open())
                    {
#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
                        if (m_streamingMode)
                        {
                            // don't constrain the decoder streaming size to 32
                            m_audioDecoder->open("", { iloj::media::AVCodec::Decoder::Stream::BestAudio });
                        }
                        else
#endif // STREAMING
                        {
                            m_audioDecoder->open("", { iloj::media::AVCodec::Decoder::Stream::BestAudio }, { 32 });
                        }
                    }
                }

                break;
            }
            case Chunk::Header::TypeId::Hevc:
            case Chunk::Header::TypeId::Vvc:
            {
                auto data_pkt = make_packet<Descriptor::Data>(std::move(pkt->getData()));
                auto videoPkt = make_packet<GenericMetadata>(); //empty

                videoPkt->contentId = static_cast<int>(pkt->getHeader().getMediaId());
                videoPkt->segmentId = static_cast<int>(pkt->getHeader().getSegmentId());

                if (m_videoDecoderList[VideoStream::Texture])
                {
                    for (std::uint32_t frameId = 0; frameId < pkt->getHeader().getNumberOfFrames(); frameId++)
                    {
                        m_videoChunkQueue.push(pkt);
                        m_genericInput.push(videoPkt);
                    }

                    m_videoDecoderList[VideoStream::Texture]->getStreamingInput().push(data_pkt);
                    if (!m_videoDecoderList[VideoStream::Texture]->is_open())
                    {
                        std::string codec_ = "hevc";
                        if (pkt->getHeader().getTypeId() == Chunk::Header::TypeId::Vvc)
                        {
                            codec_ = "vvc";
                        }

                        m_nbThread = m_configMap[codec_].m_nbThread;
                        m_hardwareDecoding = m_configMap[codec_].m_hardwareDecoding;
                        m_androidFormat = m_configMap[codec_].m_androidFormat;
#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
                        if (m_streamingMode)
                        {
                            // don't constrain the decoder streaming size to 32
                            m_videoDecoderList[VideoStream::Texture]->open("", { iloj::media::AVCodec::Decoder::Stream::BestVideo});
                        }
                        else
#endif // STREAMING
                        {
                            m_videoDecoderList[VideoStream::Texture]->open(
                                "", { iloj::media::AVCodec::Decoder::Stream::BestVideo}, {32});
                        }
                    }
                }

                break;
            }
            case Chunk::Header::TypeId::Miv:
            {
                if (m_videoDecoderList[VideoStream::Texture])
                {
                    auto [mivAU, videoDataPktList] = miv::decodeMivBuffer(
                        {reinterpret_cast<const char *>(pkt->getData().data()), pkt->getHeader().getDataSize()});

                    auto mivPkt = make_packet<GenericMetadata>(mivAU);

                    if (mivPkt)
                    {                        
                        mivPkt->contentId = static_cast<int>(pkt->getHeader().getMediaId());
                        mivPkt->segmentId = static_cast<int>(pkt->getHeader().getSegmentId());
                        

                        for (std::uint32_t frameId = 0; frameId < pkt->getHeader().getNumberOfFrames(); frameId++)
                        {
                            m_videoChunkQueue.push(pkt);
                            m_genericInput.push(mivPkt);
                        }

                        for (auto videoStreamId = 0; videoStreamId < VideoStream::Size; videoStreamId++)
                        {
                            // NOTE: Decoding only first atlas
                            if (videoDataPktList[videoStreamId])
                            {
                                //m_dashInput[videoStreamId].push(std::move(videoDataPktList[videoStreamId]));
                                m_videoDecoderList[videoStreamId]->getStreamingInput().push(
                                    std::move(videoDataPktList[videoStreamId]));

                                if (!m_videoDecoderList[videoStreamId]->is_open())
                                {
                                    m_nbThread = m_configMap["miv"].m_nbThread;
                                    m_hardwareDecoding = m_configMap["miv"].m_hardwareDecoding;
                                    m_androidFormat = m_configMap["miv"].m_androidFormat;
                                    m_videoDecoderList[videoStreamId]->open(
                                        "", { iloj::media::AVCodec::Decoder::Stream::BestVideo}, {10});
                                }
                            }
                        }
                    }
                }

                break;
            }
            case Chunk::Header::TypeId::Vpcc:
            {
                if (m_videoDecoderList[VideoStream::Texture])
                {
                    auto [framesMetadata, videoDataPktList] =
                        decodeVpccBuffer(const_cast<std::vector<uint8_t> &>(pkt->getData()));

                    if (!framesMetadata.empty())
                    {
                        //TODO streaming and reader really different ?
#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
                        if (m_streamingMode)
                        {
                            for (auto &metadata : framesMetadata)
                            {
                                auto vpccPkt = make_packet<GenericMetadata>(metadata);
                                vpccPkt->contentId = static_cast<int>(pkt->getHeader().getMediaId());
                                vpccPkt->segmentId = static_cast<int>(pkt->getHeader().getSegmentId());
                                m_genericInput.push(vpccPkt);
                                m_videoChunkQueue.push(pkt);
                            }
                        }
                        else
#endif // STREAMING
                        {
                            // keep reader behaviour for now
                            if (framesMetadata.size() == pkt->getHeader().getNumberOfFrames())
                            {
                                for (std::uint32_t frameId = 0; frameId < pkt->getHeader().getNumberOfFrames();
                                     frameId++)
                                {
                                    auto vpccPkt = make_packet<GenericMetadata>(framesMetadata[frameId]);
                                    vpccPkt->contentId = static_cast<int>(pkt->getHeader().getMediaId());
                                    vpccPkt->segmentId = static_cast<int>(pkt->getHeader().getSegmentId());

                                    m_genericInput.push(vpccPkt);
                                    m_videoChunkQueue.push(pkt); // reader behaviour: as many as vpccpkt
                                }
                            }
                        }

                        for (auto videoStreamId = 0; videoStreamId < VideoStream::Size; videoStreamId++)
                        {
                            // NOTE: Decoding only first atlas
                            if (videoDataPktList[videoStreamId])
                            {
                                //m_dashInput[videoStreamId].push(std::move(videoDataPktList[videoStreamId]));
                                m_videoDecoderList[videoStreamId]->getStreamingInput().push(
                                    std::move(videoDataPktList[videoStreamId]));
                                if (!m_videoDecoderList[videoStreamId]->is_open())
                                {
                                    m_nbThread = m_configMap["vpcc"].m_nbThread;
                                    m_hardwareDecoding = m_configMap["vpcc"].m_hardwareDecoding;
                                    m_androidFormat = m_configMap["vpcc"].m_androidFormat;

                                    m_videoDecoderList[videoStreamId]->open(
                                        "", { iloj::media::AVCodec::Decoder::Stream::BestVideo}, {10});
                                }
                            }
                        }

                        m_atlasFrameHeight = framesMetadata[0].frame_height;
                        m_atlasFrameWidth = framesMetadata[0].frame_width;

                    }
                }

                break;
            }
            case Chunk::Header::TypeId::Haptic:
            {
                if (m_hapticDecoder && m_schedulerInterface)
                {
                    using OnInit = void(std::chrono::duration<double> initTime);
                    using OnDecode = void(std::string s, HapticInput& hapticInput);
                    OnInit *onInit = nullptr;
                    OnDecode *onDecode = nullptr;
                    LoadProcEx("V3CImmersiveDecoderHaptic", "init", onInit);
                    LoadProcEx("V3CImmersiveDecoderHaptic", "decode", onDecode);
                    if (onInit != nullptr && onDecode != nullptr)
                    {
                        LOG_INFO("Haptic decoder loaded");

                        m_hapticInitTime = pkt->getHeader().getPTS();
                        m_hapticDecoder->setHapticInput(m_schedulerInterface->getHapticInput());

                        Chunk::Buffer &buf = pkt.getContent().getData();
                        std::string s(buf.begin(), buf.end());

                        onInit(m_hapticInitTime);
                        onDecode(s, m_hapticDecoder->getHapticInput());
                    }
                    else
                    {
                        LOG_WARNING("Unable to load haptic decoder");
                    }

                }
                break;
            }
            default:
                LOG_WARNING("Unknown chunk type");
                break;
        }
    }
}

void DecoderInterface::onMediaRequest(unsigned mediaId)
{
    m_requestedItemId = mediaId;
    LOG_INFO("DecoderInterface: Channel request successfully set to ", mediaId);
}

void DecoderInterface::onStart()
{

    setServiceName(L"DecoderInterface");

    m_genericInput.open();

    for (auto &videoInput : m_videoInputList)
    {
        videoInput.open();
    }
}

void DecoderInterface::initialize()
{
     LOG_INFO("DecoderInterface::initialize");
}

void DecoderInterface::idle()
{
    if (m_genericInput.wait())
    {
        auto genericPkt = m_genericInput.front();
#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
        auto is2DContent = genericPkt->contentType == GenericMetadata::ContentType::Unknown;
        auto isDASH2DContent = m_streamingMode && is2DContent;
#endif // STREAMING

        const auto &vps = genericPkt->MIVMetadata->vps;

        bool hasAtlas = !genericPkt->MIVMetadata->atlas.empty();
        auto atlasId = hasAtlas ? vps.vps_atlas_id(0) : TMIV::MivBitstream::AtlasId{};

        bool hasOccupancy = genericPkt->contentType != GenericMetadata::ContentType::VPCC
                                ? hasAtlas && vps.vps_occupancy_video_present_flag(atlasId)
                                : true;
        bool hasGeometry = genericPkt->contentType != GenericMetadata::ContentType::VPCC
                               ? hasAtlas && vps.vps_geometry_video_present_flag(atlasId)
                               : true;
        bool hasTransparency = genericPkt->contentType != GenericMetadata::ContentType::VPCC
                                   ? hasAtlas && (1 < vps.attribute_information(atlasId).ai_attribute_count())
                                   : false;

        bool is_video_ready = !(m_videoInputList[VideoStream::Texture].empty() ||
                                (hasOccupancy && m_videoInputList[VideoStream::Occupancy].empty()) ||
                                (hasGeometry && m_videoInputList[VideoStream::Geometry].empty()) ||
                                (hasTransparency && m_videoInputList[VideoStream::Transparency].empty()));
        bool is_audio_ready = !(m_audioChunkQueue.empty());

        if (is_video_ready)
        {
            std::array<VideoPacket, VideoStream::Size> videoPacketList;
            {
                using namespace std::chrono_literals;

                //decoding FPS measure
                if (m_measureFPS)
                {
                    
                    auto now = std::chrono::high_resolution_clock::now();
                    m_deltaTpkt = now - m_Tpkt;
                    m_Tpkt = now;

                    m_queueMutex.lock();
                    m_queueDTpkt.push(m_deltaTpkt);
                    if (m_queueDTpkt.size() > m_queueMaxSize)
                    {
                        while (!m_queueDTpkt.empty())
                        {
                            m_queueDTpkt.pop();
                        }
                        LOG_WARNING("Flushing Decoder FPS Measures as they were not consumed. If you don't need FPS "
                                    "measure, deactivate the option in config.json");
                    }
                    m_queueMutex.unlock();
                }

                std::chrono::duration<double> pts;
                auto &header = m_videoChunkQueue.front()->getHeader();

#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
                if (m_streamingMode)
                {
                    // save the very first PTS value.
                    if (m_streamingFirstPtsVideo)
                    {
                        m_firstOriginPTS = header.getPTS();
                        m_streamingFirstPtsVideo = false;
                        auto myinitTime = std::chrono::duration_cast<std::chrono::milliseconds>(m_firstOriginPTS);
                        LOG_INFO("video idle, very 1st PTS m_firstOriginPTS= ", std::abs(myinitTime.count()));
                    }
                    else
                    {
                        m_firstOriginPTS += header.getDuration();
                        auto initTime = std::chrono::duration_cast<std::chrono::milliseconds>(m_firstOriginPTS);
                        //LOG_INFO("video idle pts= ", std::abs(initTime.count()), ", duration= ", header.getDuration());
                    }
                    pts = m_firstOriginPTS;
                    header.setPTS(pts);
                }
                else
#endif // STREAMING
                {
                    pts = header.getPTS();
                    auto duration = header.getDuration() / header.getNumberOfFrames();
                    header.setPTS(pts + duration);
                }

                videoPacketList[VideoStream::Texture] = m_videoInputList[VideoStream::Texture].front();

                videoPacketList[VideoStream::Texture]->getMetadata().setTimeStamp(pts);
                videoPacketList[VideoStream::Texture]->getMetadata().set<std::uint16_t>(header.getMediaId());
 
#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
                if (!m_streamingMode || !is2DContent || (isDASH2DContent && m_videoChunkQueue.size() > 1))
#endif // STREAMING
                {
                    m_videoChunkQueue.pop();
                }

                m_videoInputList[VideoStream::Texture].pop();
            }

            if (hasOccupancy)
            {
                videoPacketList[VideoStream::Occupancy] = m_videoInputList[VideoStream::Occupancy].front();
                m_videoInputList[VideoStream::Occupancy].pop();
            }

            if (hasGeometry)
            {
                videoPacketList[VideoStream::Geometry] = m_videoInputList[VideoStream::Geometry].front();
                m_videoInputList[VideoStream::Geometry].pop();
            }

            if (hasTransparency)
            {
                videoPacketList[VideoStream::Transparency] = m_videoInputList[VideoStream::Transparency].front();
                m_videoInputList[VideoStream::Transparency].pop();
            }

            if (m_schedulerInterface)
            {
                DecodedVideoData data = {std::move(genericPkt), std::move(videoPacketList)};
                m_schedulerInterface->getVideoInput().push(make_packet<DecodedVideoData>(std::move(data)));

#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
                if (!m_streamingMode || !is2DContent || (isDASH2DContent && m_genericInput.pending() > 1))
#endif // STREAMING
                {
                    m_genericInput.pop();
                }
            }
        }
#if 0
        else if (is_audio_ready)
        {
            std::chrono::duration<double> pts;
            auto &header = m_audioChunkQueue.front()->getHeader();
#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
            if (m_streamingMode)
            {
                if (m_streamingFirstFrame)
                {
                    pts = std::chrono::system_clock::now().time_since_epoch() + 1s;
                    m_streamingFirstFrame = false;
                }
                else
                {
                    pts = m_streamingFirstFrame + header.getDuration();
                }
                m_streamingFirstFrame = pts;
            }
            else
#endif // STREAMING
            {
                pts = header.getPTS();
                auto duration = header.getDuration() / header.getNumberOfFrames();
                header.setPTS(pts + duration);
            }
        }
#endif
        else
        {
            std::this_thread::sleep_for(std::chrono::microseconds{100});
        }
    }
}

void DecoderInterface::finalize()
{
    stopDecoders();

    LOG_INFO("DecoderInterface::finalize");
}

void DecoderInterface::allocateAudioDecoder(std::string avcodec_name)
{
    m_audioDecoder = std::make_unique<iloj::media::AVCodec::Decoder>();
    m_audioDecoder->init(avcodec_name);

    m_audioDecoder->setOnOpeningFunction(
        [this]()
        {
            // Connection to scheduler
            if (m_schedulerInterface)
            {
                connect(m_audioDecoder->getAudioOutput(0), m_schedulerInterface->getAudioInput());
            }

            // On frame callback
            m_audioDecoder->setOnAudioFrameCallback(0,
                [this](Descriptor::Audio& desc)
                {
                    if (m_audioChunkQueue.empty())
                    {
                        //LOG_INFO("AUDIO: m_audioChunkQueue empty");
                    }
                    else
                    {
                        std::chrono::duration<double> pts;
                        auto &header = m_audioChunkQueue.front()->getHeader();
#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
                        if (m_streamingMode)
                        {
                            if (m_streamingFirstFrameAudio)
                            {
                                pts = std::chrono::system_clock::now().time_since_epoch() + 1s;
                                m_streamingFirstFrameAudio = false;
                            }
                            else
                            {
                                pts = m_streamingFramePTSAudio + header.getDuration();
                            }
                            m_streamingFramePTSAudio = pts;
                            int ptsRound = (int)round(pts.count());
                            std::string ptsStr = std::to_string(ptsRound);
                            std::string ptsLatestStr = ptsStr.substr(ptsStr.size() - 6);
                        }
                        else
#endif // STREAMING
                        {
                            pts = header.getPTS();
                            auto duration = header.getDuration() / header.getNumberOfFrames();
                            header.setPTS(pts + duration);
                        }

                        desc.getMetadata().setTimeStamp(pts);
                        desc.getMetadata().set<std::uint16_t>(header.getMediaId());
                        m_audioChunkQueue.pop();
                    }
                });

            LOG_INFO("Audio stream opened");

            // Starting
            m_audioDecoder->start();

            LOG_INFO("Audio decoder started");
        });

}

void DecoderInterface::allocateHapticDecoder()
{
    m_hapticDecoder = std::make_unique<HapticDecoder>();
}

void DecoderInterface::allocateVideoDecoders(std::string avcodec_name)
{
    // VideoStream: Occupancy, Geometry, Texture, Transparency
    for (auto videoStreamId = 0; videoStreamId < VideoStream::Size; videoStreamId++)
    {
        m_videoDecoderList[videoStreamId] = std::make_unique<iloj::media::AVCodec::Decoder>();
        m_videoDecoderList[videoStreamId]->init(avcodec_name);

        m_videoDecoderList[videoStreamId]->setOnOpeningFunction(
            [this, videoStreamId]()
            {
                // Connection to internal input
                connect(m_videoDecoderList[videoStreamId]->getVideoOutput(0, m_nbThread, m_hardwareDecoding, m_androidFormat, *g_procVideoDecodingList[videoStreamId]), m_videoInputList[videoStreamId]);

                LOG_INFO(miv::getVideoStreamName(videoStreamId), " stream opened");

                // Starting
                m_videoDecoderList[videoStreamId]->start();

                LOG_INFO(miv::getVideoStreamName(videoStreamId), " decoder started");
            });
    }
}

void DecoderInterface::stopDecoders()
{
    // finish is not blocking and asks the decoders to end their execution loop
    // by the time the blocking call to stop is issued, the decoders could already
    // be ready to join.
    m_audioDecoder->finish();
    for (auto &dec : m_videoDecoderList)
        dec->finish();
    stopHapticDecoder();
    stopAudioDecoder();
    stopVideoDecoders();
}

void DecoderInterface::stopHapticDecoder()
{
    LOG_INFO("Haptic decoder stopped");
}

void DecoderInterface::stopAudioDecoder()
{
    m_audioDecoder->getStreamingInput().push(Packet<Descriptor::Data>{});
    m_audioDecoder->stop();
    m_audioDecoder->exit();

    LOG_INFO("Audio queue size: ", m_audioChunkQueue.size());
    LOG_INFO("Audio decoder stopped");

    m_audioChunkQueue = {};
}

void DecoderInterface::stopVideoDecoders()
{
    m_genericInput.clear();

    for (auto videoStreamId = 0; videoStreamId < VideoStream::Size; videoStreamId++)
    {
        m_videoInputList[videoStreamId].clear();

        m_videoDecoderList[videoStreamId]->stop();

        m_videoDecoderList[videoStreamId]->exit();

        LOG_INFO(miv::getVideoStreamName(videoStreamId), " decoder stopped");
    }

    LOG_INFO("Video queue size: ", m_videoChunkQueue.size());
    LOG_INFO("Video decoders stopped");

    m_videoChunkQueue = {};
}

