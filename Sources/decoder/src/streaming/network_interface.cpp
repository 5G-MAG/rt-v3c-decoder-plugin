/*
* Copyright (c) 2024 InterDigital
* Licensed under the License terms of 5GMAG software (the "License").
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at https://www.5g-mag.com/license .
* Unless required by applicable law or agreed to in writing, software distributed under the License is
* distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and limitations under the License.
*/


#include <streaming/network_interface.h>
#include <iloj/misc/filesystem.h>
#include <iloj/misc/logger.h>
#include <iloj/misc/dll.h>

using namespace iloj::misc; 

std::string NetworkInterface::readHostAddress(const JSON::Object &serverProperties) const
{
    using namespace JSONSchemaKeywords::Networking;

    std::string res;
    if (serverProperties.hasItem(IP) && serverProperties.hasItem(Port)) {
        std::ostringstream strstream;
        strstream << "http://" << serverProperties.getItem<JSON::String>(IP).getValue() << ':'
                  << serverProperties.getItem<JSON::Integer>(Port).getValue() << '/';
        res = strstream.str();
    }
    return res;
}

void NetworkInterface::readNetworkConfig(const JSON::Object &jsonNetworking)
{
    using namespace JSONSchemaKeywords::Networking;

    if (!jsonNetworking.hasItem(Servers) && !jsonNetworking.hasItem(Senders))
        return;
    
#if defined DASH_STREAMING
    // DASH streaming
    if (jsonNetworking.hasItem(Servers))
    {
        auto& servers = jsonNetworking.getItem<JSON::Array>(Servers);
        if (!servers.getSize())
            return;

        for (auto& item : servers.getItems()) {
            auto& serverProps = item->castTo<JSON::Object>();
            auto& serverName =
                serverProps.hasItem(Name) ? serverProps.getItem<JSON::String>(Name).getValue() : DefaultServerName;
            m_remoteHosts.emplace(serverName, readHostAddress(serverProps));
        }
    }
#endif

#if defined UVG_RTP_STREAMING
    // RTP UVG
    if (jsonNetworking.hasItem(Senders))
    {
        std::string ipAddr;
        int iPort;

        auto& rtpSender = jsonNetworking.getItem<JSON::Array>(Senders);
        if (!rtpSender.getSize())
            return;

        for (auto& item : rtpSender.getItems()) {
            auto& rtpSenderProps = item->castTo<JSON::Object>();
            auto& senderName =
                rtpSenderProps.hasItem(Name) ? rtpSenderProps.getItem<JSON::String>(Name).getValue() : DefaultSenderName;

            if (rtpSenderProps.hasItem(IP))
            {
                ipAddr = rtpSenderProps.getItem<JSON::String>(IP).getValue();
            }
            else
            {
                LOG_ERROR("RTP session information is wrong, sender has no IP address");
            }
            if (rtpSenderProps.hasItem(Port))
            {
                iPort = rtpSenderProps.getItem<JSON::Integer>(Port).getValue();
            }
            else
            {
                LOG_ERROR("RTP session information is wrong, sender has no port value");
            }
            m_rtpSender.emplace(senderName, std::make_pair(ipAddr, iPort));
        }
    }
    else
    {
        LOG_ERROR("RTP session information is wrong, there is no RTP sender information");
    }

    if (jsonNetworking.hasItem(Receiver))
    {
        std::string ipAddr;
        int iPort;
        
        auto& rtpReceiver = jsonNetworking.getItem<JSON::Array>(Receiver);
        if (!rtpReceiver.getSize())
            return;

        auto& item = rtpReceiver.getItem(0);
        auto& rtpReceiverProps = item.castTo<JSON::Object>();

        if (rtpReceiverProps.hasItem(IP))
        {
            ipAddr = rtpReceiverProps.getItem<JSON::String>(IP).getValue();
        }
        else
        {
            LOG_ERROR("RTP session information is wrong, receiver has no IP address");
        }
        if (rtpReceiverProps.hasItem(Port))
        {
            iPort = rtpReceiverProps.getItem<JSON::Integer>(Port).getValue();
        }
        else
        {
            LOG_ERROR("RTP session information is wrong, receiver has no port value");
        }

        m_rtpReceiver = std::make_pair(ipAddr, iPort);
    }
    else
    {
        LOG_ERROR("RTP session information is wrong, there is no RTP receiver information");
    }

#endif

    // RTP WebRTC
    //TODO

    return;
}

void NetworkInterface::updateStreamData(unsigned int mediaId)
{
    m_currentMediaId = mediaId;

#if defined DASH_STREAMING
    if (m_itemList[mediaId].getMode().compare("dash") == 0)
    {
        LOG_INFO("update Network settings for DASH ... ");
        const auto& stream = getItemStream(m_currentMediaId);
        // retrieve the designated server of the current media
        const auto& remoteHost = m_remoteHosts[stream.getServerNameRef()];
        // assemble the url of the media
        m_dashSegmentReceiver.m_mpdURL = remoteHost + stream.getUrlRef();
    }
#endif
#if defined UVG_RTP_STREAMING
    if (m_itemList[mediaId].getMode().compare("rtp") == 0)
    {
        LOG_INFO("Update Network settings for UVG RTP ... ");
        const auto& stream = getItemStream(m_currentMediaId);
        const auto& rtpSender = m_rtpSender[stream.getSenderName()];
        m_rtpPacketReceiver.m_rtpSessionSender = rtpSender;
        m_rtpPacketReceiver.m_rtpSessionReceiver = m_rtpReceiver;
    }
#endif
#if defined WEBRTC_RTP_STREAMING
    if (m_itemList[mediaId].getMode().compare("webrtc") == 0)
    {
        LOG_INFO("Update Network settings for WebRTC RTP ... ");
        //TODO
    }
#endif


    return;
}

void NetworkInterface::startAll()
{

#if defined DASH_STREAMING
    if (!m_protocolOnService.compare("dash")) {
        m_dashSegmentReceiver.start();
    }
#endif
#if defined UVG_RTP_STREAMING
    if (!m_protocolOnService.compare("rtp")) {
        m_rtpPacketReceiver.start();
        LOG_INFO("Configure UVG RTP receiver ... ");
        //TODO
    }
#endif
#if defined WEBRTC_RTP_STREAMING
    if (!m_protocolOnService.compare("webrtc")) {
        LOG_INFO("Configure UVG RTP receiver ... ");
        //TODO;
    }
#endif

    return;
}

void NetworkInterface::stopAll()
{
#if defined DASH_STREAMING
    if (!m_protocolOnService.compare("dash")) {
        LOG_INFO("Stop DASH segment receiver ... ");
        m_dashSegmentReceiver.stop();
    }
#endif
#if defined UVG_RTP_STREAMING
    if (!m_protocolOnService.compare("rtp")) {
        LOG_INFO("Stop UVG RTP receiver ... ");
        m_rtpPacketReceiver.stop();
        //TODO
    }
#endif
#if defined WEBRTC_RTP_STREAMING
    if (!m_protocolOnService.compare("webrtc")) {
        LOG_INFO("Stop UVG RTP receiver ... ");
        //TODO
    }
#endif

    return;
}

void NetworkInterface::stopServices()
{
    if (running()) {
        stop();    // stop the service thread
        stopAll(); // stop all the other sub services
    }
    return;
}

void NetworkInterface::exitIdle()
{
    m_closing = true;
}

void NetworkInterface::onConfigure(const std::string& configFile)
{
    LOG_INFO("NetworkInterface::onConfigure ", configFile);
    
    using namespace JSONSchemaKeywords::Networking;

    auto jsonConfig = JSON::Object::fromFile(configFile);
    if (jsonConfig.isEmpty()) {
        LOG_ERROR("Config file not found or unreadable");
        return;
    }

    const auto jsonNetworking = jsonConfig.getItem<JSON::Object>("Networking");
    if (jsonNetworking.isEmpty()) {
        LOG_ERROR("Networking configuration missing from config file");
        return;
    }
    readNetworkConfig(jsonNetworking);

    //TODO check pair is empty
    //if (m_rtpSessionSender.empty() || m_rtpSessionReceiver.empty()) {
    //    LOG_ERROR("No available RTP session information in config file");
    //    return;
    //}
    //TODO difference between DASH and RTP in json files, config.json and or library.json
#if defined DASH_STREAMING
    if (jsonNetworking.hasItem(Servers))
    {
        if (m_remoteHosts.empty()) {
            LOG_ERROR("No available host address in config file");
            return;
        }

        const auto libraryPath = FileSystem::Path::getAbsolute(
            { jsonConfig.getItem<JSON::String>("Library").getValue(), FileSystem::Path{configFile}.getParent() });
        if (!FileSystem::File{ libraryPath }.exist()) {
            LOG_ERROR("Library file not found: ", libraryPath.toString());
            return;
        }

        auto jsonLibrary = JSON::Object::fromFile(libraryPath.toString());
        if (jsonLibrary.isEmpty()) {
            LOG_ERROR("Library is unreadable");
            return;
        }

        const auto& jsonPlaylist = jsonLibrary.getItem<JSON::Array>("Playlist");
        const auto nbChannel = jsonPlaylist.getSize();
        if (!nbChannel) {
            LOG_ERROR("Playlist data is either missing or empty");
            return;
        }

        m_itemList = Item::makeItemList(jsonLibrary, nbChannel, true);
        if (m_itemList.empty()) {
            LOG_ERROR("Playlist is empty or unreadable");
            return;
        }

        for (auto& item : m_itemList)
            m_mediaList.emplace_back(item.getName());
        m_bufferCapacity = std::max(3, jsonNetworking.getItem<JSON::Integer>("SegmentsBufferCapacity").getValue());
    }
    else
    {
        m_bufferCapacity = 3;
    }
#endif

#if defined UVG_RTP_STREAMING
    if (jsonNetworking.hasItem(Senders))
    {
        if (m_rtpSender.empty()) {
            LOG_ERROR("No available sender information in config file");
            return;
        }
        const auto libraryPath = FileSystem::Path::getAbsolute(
            { jsonConfig.getItem<JSON::String>("Library").getValue(), FileSystem::Path{configFile}.getParent() });
        if (!FileSystem::File{ libraryPath }.exist()) {
            LOG_ERROR("Library file not found: ", libraryPath.toString());
            return;
        }

        auto jsonLibrary = JSON::Object::fromFile(libraryPath.toString());
        if (jsonLibrary.isEmpty()) {
            LOG_ERROR("Library is unreadable");
            return;
        }

        const auto& jsonPlaylist = jsonLibrary.getItem<JSON::Array>("Playlist");
        const auto nbChannel = jsonPlaylist.getSize();
        if (!nbChannel) {
            LOG_ERROR("Playlist data is either missing or empty");
            return;
        }

        m_itemList = Item::makeItemList(jsonLibrary, nbChannel, true);
        if (m_itemList.empty()) {
            LOG_ERROR("Playlist is empty or unreadable");
            return;
        }
    }
#endif

}

void NetworkInterface::onStartEvent(unsigned mediaId)
{
    LOG_INFO("NetworkInterface::onStartEvent ");
    bool dllLoaded = false;

#if defined DASH_STREAMING
    if (m_itemList[mediaId].getMode().compare("dash") == 0)
    {
        LOG_INFO("Load V3C DASH Streamer dll ... ");
        dllLoaded = m_dashSegmentReceiver.loadDll();
        m_protocolOnService = "dash";

        m_decoder = static_cast<DecoderInterface*>(m_decoderInterface);
        m_dashSegmentReceiver.setDecoderInterface(m_decoder);
    }
#endif
#if defined UVG_RTP_STREAMING
    if (m_itemList[mediaId].getMode().compare("rtp") == 0)
    {
        LOG_INFO("Load UVG RTP dll ... ");
        dllLoaded = m_rtpPacketReceiver.loadDll();
        m_protocolOnService = "rtp";
    }
#endif
#if defined WEBRTC_RTP_STREAMING
    if (m_itemList[mediaId].getMode().compare("webrtc") == 0)
    {
        LOG_INFO("Load WebRTC RTP dll ... ");
        //TODO
        // dllLoaded = m_webRTCReceiver.loadDll();
        m_protocolOnService = "webrtc";
    }
#endif

    if (dllLoaded==true)
    {
        LOG_INFO("Dll loaded");

        m_closing = false;
        m_requestMediaId = mediaId;
        updateStreamData(m_requestMediaId);
        // cast the decoder interface
        if (!m_decoderInterface)
        {
            LOG_ERROR("Decoder not present");
            return;
        }

        m_t0 = m_timer.restart();

        start();
    }
    else
    {
        LOG_WARNING("Unable to load dll");
        if (m_decoderInterface)
        {
            //m_decoderInterface->onErrorEvent(level, (unsigned int)dashChunk->errorStreamer);
            //m_decoderInterface->onErrorEvent(2, (unsigned int)0);
            auto cb = m_decoderInterface->getOnErrorEventCallback();
            if (cb != nullptr)
            {
                cb(2, (unsigned int)0);
            }
        }
        return;
    }
}

void NetworkInterface::onMediaRequest(unsigned mediaId)
{
    m_requestMediaId = mediaId;

    m_closing = false;

#if defined DASH_STREAMING
    m_circularBufferIsFull = false;
#endif
#if defined UVG_RTP_STREAMING
    m_uvgRTPReceiverIsStarted = false;
#endif
}

void NetworkInterface::initialize()
{
    LOG_INFO("NetworkInterface::initialize");

    m_t0 = m_timer.restart();

#if defined DASH_STREAMING
    m_circularBufferIsFull = false;
#endif
#if defined UVG_RTP_STREAMING
    m_uvgRTPReceiverIsStarted = false;
#endif
}

void NetworkInterface::idle()
{
    try
    {
#if defined DASH_STREAMING
        if (!m_protocolOnService.compare("dash"))
        {
            updateItemForDash();
        }
#endif
#if defined UVG_RTP_STREAMING
        if (!m_protocolOnService.compare("rtp"))
        {
            updateItemForUvgRtp();
        }
#endif
#if defined WEBRTC_RTP_STREAMING
        if (!m_protocolOnService.compare("webrtc"))
        {
            updateItemForWebRtc();
        }
#endif
    }
    catch (std::exception e)
    {
        LOG_ERROR(e.what());
        throw;
    }
}

void NetworkInterface::updateItemForDash()
{
#if defined DASH_STREAMING

    if (m_closing)
        return;

    // DASH client circular buffer is full.
    // We can start consuming chunks
    if (m_dashSegmentReceiver.dashReceiverStarted() && !m_circularBufferIsFull)
    {
        m_circularBufferIsFull = true;
        m_t0 = m_timer.restart();
        m_delay = std::chrono::duration<double>::zero();
        m_originPTS = std::chrono::system_clock::now().time_since_epoch() + m_lookAhead;
        auto myPTS = std::chrono::duration_cast<std::chrono::milliseconds>(m_originPTS);
        LOG_INFO("Idle epoch pts= ", std::abs(myPTS.count()));
        m_videoPTSIsInitialized = false;
        m_hapticPTSIsInitialized = false;
    }

    // in the thread loop, this condition gives the chunk consumption and delivery pace.
    if (std::chrono::system_clock::now().time_since_epoch() >= m_checkpoint && m_circularBufferIsFull)
    {
        Chunk chunk(std::move(m_dashSegmentReceiver.getMediaChunk()));

        // finalize the header
        auto& header = chunk.getHeader();

        // error handling
        if (header.getErrorStreamer() != 0)
        {
            DASHCcode keyD = static_cast<DASHCcode>(header.getErrorStreamer());
            CURLcode keyC = static_cast<CURLcode>(m_dashSegmentReceiver.getDashNetworkStatus());

            std::string logMsgD = dashCodeMap.at(keyD);
            std::string logMsgC = curlCodeMap.at(keyC);

            LOG_ERROR("RunTime Streamer Error = ", logMsgD);
            LOG_ERROR("Network status, ", logMsgC);
            LOG_ERROR("HTTP status, ", m_dashSegmentReceiver.getDashHTTPStatus());

            // by default error handling is a stop of the app
            // but in case of bad network conditions, the level is set to 0
            // to give a chance to retrieve better conditions.
            int level = 2;
            if ((keyD == DASHCcode::DASHCLIENT_ACCESS_TO_SEGMENT_FAILED)
                && (keyC == CURLcode::CURL_COULDNT_CONNECT) || (keyC == CURLcode::CURL_OPERATION_TIMEDOUT))
            {
                level = 0;
            }
            //TODO reconnect error event through client interface
            if (m_decoderInterface)
            {
                //m_decoderInterface->onErrorEvent(level, (unsigned int)header.getErrorStreamer());
                auto cb = m_decoderInterface->getOnErrorEventCallback();
                if (cb != nullptr)
                {
                    cb(level, (unsigned int)header.getErrorStreamer());
                }
            }
        }

        header.setMediaId(m_currentMediaId);

        // update the checkpoint for the next loop
        auto segDuration = std::chrono::duration<double>(header.getSegmentDuration());
        m_checkpoint = m_t0.time_since_epoch() + m_delay;
        m_delay += segDuration;

        // set the origin PTS for the first video chunk.
        // this PTS value is then incremented by the decoder (idle) with the frame duration value.
        if ((!m_videoPTSIsInitialized) &&
            ((header.getTypeId() == Chunk::Header::TypeId::Hevc)
                || (header.getTypeId() == Chunk::Header::TypeId::Vvc)
                || (header.getTypeId() == Chunk::Header::TypeId::Miv)
                || (header.getTypeId() == Chunk::Header::TypeId::Vpcc)))
        {
            header.setPTS(m_originPTS);
            m_videoPTSIsInitialized = true;
        }
        // set the origin PTS for the haptic content.
        // origin PTS are identical for video and haptic.
        if (header.getTypeId() == Chunk::Header::TypeId::Haptic)
        {
            if (!m_hapticPTSIsInitialized)
            {
                header.setPTS(m_originPTS);
                m_hapticPTSIsInitialized = true;
            }
            else
            {
                // next loop, the origin PTS is incremented with the duration that
                // corresponds to all haptic effects described in the .hjif file.
                header.setPTS(m_originPTS + header.getDuration());
                m_originPTS = header.getPTS();
            }
            auto myPTS = std::chrono::duration_cast<std::chrono::milliseconds>(m_originPTS);
            LOG_INFO("haptic origin pts= ", std::abs(myPTS.count()));
        }

        // number of frames: only MIV needs to keep its number of frames in dash mode to keep the video data
        // synchronised with the V3C data
        if ((header.getTypeId() != Chunk::Header::TypeId::Miv)
            && (header.getTypeId() != Chunk::Header::TypeId::Haptic)
            )
            header.setNumberOfFrames(1);

        if (m_decoderInterface)
        {
            m_decoderInterface->onChunkEvent(std::move(chunk));
        }
    }
#endif
}

void NetworkInterface::updateItemForUvgRtp()
{
#if defined UVG_RTP_STREAMING

    if (m_closing)
        return;

    if (m_rtpPacketReceiver.uvgRTPReceiverStarted() && !m_uvgRTPReceiverIsStarted)
    {
        m_uvgRTPReceiverIsStarted = true;
    }

    if (!m_uvgRTPReceiverIsStarted)
        return;

    // in the thread loop, this condition gives the chunk consumption and delivery pace.
    //if (std::chrono::system_clock::now().time_since_epoch() >= m_checkpoint)
    if (m_rtpPacketReceiver.receiverIsReady() && m_uvgRTPReceiverIsStarted)
    {
        LOG_INFO("RTP packet received ... ");

        // check the origin PTS is already set, otherwise set it
        if (m_originPTS.count() == 0.0)
        { 
            m_t0 = m_timer.restart();
            m_delay = std::chrono::duration<double>::zero();
            m_originPTS = std::chrono::system_clock::now().time_since_epoch() + m_lookAhead;
            auto myPTS = std::chrono::duration_cast<std::chrono::milliseconds>(m_originPTS);
            LOG_INFO("Idle epoch pts= ", std::abs(myPTS.count()));
            m_videoPTSIsInitialized = false;
        }

        //m_rtpPacketReceiver.getMediaChunk();
        Chunk chunk(std::move(m_rtpPacketReceiver.getMediaChunk()));

        // finalize the header
        auto& header = chunk.getHeader();

        // error handling
        //TODO

        header.setMediaId(m_currentMediaId);

        // update the checkpoint for the next loop
        //auto segDuration = std::chrono::duration<double>(header.getSegmentDuration());
        //m_checkpoint = m_t0.time_since_epoch() + m_delay;
        //m_delay += segDuration;
        
        // frame duration in RTP????

        if ((!m_videoPTSIsInitialized) &&
            ((header.getTypeId() == Chunk::Header::TypeId::Hevc)
                || (header.getTypeId() == Chunk::Header::TypeId::Vvc)
                || (header.getTypeId() == Chunk::Header::TypeId::Miv)
                || (header.getTypeId() == Chunk::Header::TypeId::Vpcc)))
        {
            header.setPTS(m_originPTS);
            m_videoPTSIsInitialized = true;
        }

        if (m_decoderInterface)
        {
            m_decoderInterface->onChunkEvent(std::move(chunk));
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });
#endif
}

void NetworkInterface::updateItemForWebRtc()
{
#if defined WEBRTC_RTP_STREAMING

    if (m_closing)
        return;
    //TODO
#endif
}