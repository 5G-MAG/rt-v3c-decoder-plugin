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

#pragma once

#include <common/misc/spsc_queue.h>
#include <common/stream/item.h>
#include <decoder/decoder.h>
#include <iloj/misc/json.h>
#include <iloj/misc/socket.h>
#include <iloj/misc/thread.h>
#include <iloj/misc/time.h>
#include <interface/client.h>
#include <streaming/client_dash.h>
#if defined UVG_RTP_STREAMING
#include <streaming/receiver_uvg_rtp.h>
#endif
#include <string>
#include <chrono>

typedef std::chrono::steady_clock::time_point timePoint;


class NetworkInterface: public Client::Interface, public iloj::misc::Service
{
    //////////////////////////////////
    /// ClientInterface members
    //////////////////////////////////
private:
    /// <summary>
    /// the provider of Chunks
    /// the running cycle of this Service is controlled by the ClientInterface
    /// </summary>
#if defined DASH_STREAMING
    DashSegmentReceiver m_dashSegmentReceiver;
#endif
#if defined UVG_RTP_STREAMING
    RtpPacketReceiver m_rtpPacketReceiver;
#endif

    std::string m_protocolOnService{ "dash" };

    unsigned int m_bufferCapacity{ 3 };
    std::chrono::duration<double> m_checkpoint;
    std::chrono::duration<double> m_delay{ 0 };
    std::chrono::milliseconds m_lookAhead{ 1000 };
    bool m_circularBufferIsFull{ false };

    /// <summary>
    /// Client::Interface members
    /// </summary>
    std::vector<std::string> m_mediaList;
    unsigned int m_currentMediaId{0};
    /// <summary>
    /// control of the idle loop exit
    /// </summary>
    std::atomic_bool m_closing{false};
    /// <summary>
    /// List of Items containing properties of the contents, used by ClientInterface:
    /// - media name (must be the same in the themes file)
    /// - server name
    /// - url on the server
    /// - framerate
    /// </summary>
    std::vector<Item> m_itemList;
    /// <summary>
    /// hold the id of the requested media until the update of the current media can be done
    /// </summary>
    std::size_t m_requestMediaId{0};
    std::size_t m_currentItemId{};

    /// <summary>
    /// cache the TypeId of the current media
    /// </summary>
    Chunk::Header::TypeId m_typeId;

    /// <summary>
    /// map of the configured DASH servers IPs and port
    /// <server name, server address>
    /// the format of server address is: http://IP:Port/
    /// </summary>
    std::map<std::string, std::string> m_remoteHosts;

    std::map<std::string, std::pair<std::string, int>> m_rtpSender;
    std::pair<std::string, int> m_rtpReceiver;

    iloj::misc::Timer<std::chrono::system_clock> m_timer;
    iloj::misc::Timer<std::chrono::system_clock>::time_point m_t0;
    bool m_videoPTSIsInitialized{false};
    bool m_hapticPTSIsInitialized{false};
    std::chrono::duration<double> m_originPTS{ 0.0 };
#ifdef MEASUREMENT_LOG
    long long m_previous_tp{ 0 };
#endif
    bool m_uvgRTPReceiverIsStarted{ false };

    void initialize() override;
    void idle() override;
    void finalize() override { LOG_INFO("NetworkInterface::finalize"); }
    void onStart() override { startAll(); }
    void onStop() override { exitIdle(); }

    /// <summary>
    /// Parse a server address from a server properties JSON::Object
    /// </summary>
    std::string readHostAddress(const iloj::misc::JSON::Object &serverProperties) const;
    /// <summary>
    /// Parse the Networking JSON::Object of the config file to fill in m_itemList and m_remoteHosts
    /// </summary>
    void readNetworkConfig(const iloj::misc::JSON::Object &jsonNetworking);
    /// <summary>
    /// shortcut to the Video Stream of an Item in m_itemList
    /// the Video Stream contains:
    /// - typeId
    /// - framerate
    /// - server name
    /// </summary>
    const Item::Stream &getItemStream(unsigned int mediaId) const { return m_itemList[mediaId].getStreams()[0]; }
    /// <summary>
    /// called at the start and at a media request
    /// update the following:
    /// - m_currentMediaId
    /// - m_typeId
    /// - m_frametime
    ///
    /// pass the typeId data to the producer
    /// </summary>
    void updateStreamData(unsigned int mediaId);
    /// <summary>
    /// start the producer subservice, which in turn starts its own producer's subservice
    /// </summary>
    void startAll();
    /// <summary>
    /// stop the producer, clear the input and stop the DASH streamer
    /// </summary>
    void stopAll();
    /// <summary>
    /// stop this Service and the Services of the producers, sequentially
    /// the function returns after every service is effectively stopped
    /// </summary>
    void stopServices();
    /// <summary>
    /// actions to perform in order to exit the idle loop without locking
    /// </summary>
    void exitIdle();

    void updateItemForDash();
    void updateItemForUvgRtp();
    void updateItemForWebRtc();

public:
    //DecoderInterface* m_decoder = static_cast<DecoderInterface*>(m_decoderInterface);
    DecoderInterface* m_decoder{ nullptr };
    /// <summary>
    /// Client::Interface implementation
    /// </summary>
    auto getMediaList() -> const std::vector<std::string> & override { return m_mediaList; }
    int getMediaId() const override { return m_currentMediaId; }
    void onConfigure(const std::string &configFile) override;
    void onStartEvent(unsigned mediaId) override;
    void onMediaRequest(unsigned mediaId) override;
    void onStopEvent() override
    {
        LOG_INFO("NetworkInterface::onStopEvent");
        stopServices();
    }
};


namespace JSONSchemaKeywords::Networking
{
const std::string Senders{ "Senders" };
const std::string Servers{"Servers"};
const std::string Receiver{ "Receiver" };
const std::string IP{"IP"};
const std::string Port{"Port"};
const std::string Name{"Name"};
const std::string DefaultServerName{"local"};
const std::string DefaultSenderName{ "localhost" };
} // namespace JSONSchemaKeywords::Networking