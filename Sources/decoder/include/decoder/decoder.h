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

#pragma once

#include <iloj/media/avcodec.h>
#include <interface/decoder.h>
#include <decoder/decoder_haptic.h>
#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
#include <interface/client.h>
#ifdef MEASUREMENT_LOG
typedef std::chrono::steady_clock::time_point timePoint;
#endif // MEASUREMENT_LOG
#endif // STREAMING
#include <common/stream/item.h>


class DecoderInterface: public Decoder::Interface, public iloj::misc::Service
{
private:
    static std::array<std::unique_ptr<iloj::gpu::Processor>, VideoStream::Size> g_procVideoDecodingList;

private:

    struct Config
    {
        std::string m_name{};
        unsigned m_nbThread{};
        bool m_hardwareDecoding{};
        std::string m_androidFormat{};
    };

    bool m_glInteroperability{};
    std::map<std::string,Config> m_configMap;
    std::string m_avcodec_name{};

    int m_nbThread{};
    bool m_hardwareDecoding{};
    std::string m_androidFormat{};

    std::unique_ptr<HapticDecoder> m_hapticDecoder;
    std::queue<HapticPacket> m_hapticChunkQueue;

    std::unique_ptr<iloj::media::AVCodec::Decoder> m_audioDecoder;
    std::queue<iloj::misc::Packet<Chunk>> m_audioChunkQueue;

    std::array<std::unique_ptr<iloj::media::AVCodec::Decoder>, 4> m_videoDecoderList;
    std::queue<iloj::misc::Packet<Chunk>> m_videoChunkQueue;

    iloj::misc::Input<GenericMetadata> m_genericInput;
    std::array<VideoInput, 4> m_videoInputList;

    iloj::misc::SpinLock m_locker;

    unsigned m_requestedItemId{0};

    std::vector<Item> m_itemList;

    //V3C frame decoding FPS measure

    bool m_measureFPS = false;

    std::chrono::duration<double> m_deltaTpkt{0};
    std::chrono::duration<double> m_Tpkt{0};
    std::queue<std::chrono::duration<double>> m_queueDTpkt;
    std::mutex m_queueMutex;
    int m_queueMaxSize = 1000;

    //Needed when rendering outside the plugin, as we need to allocate textures the right size for the content
    int m_atlasFrameWidth;
    int m_atlasFrameHeight;

#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
    bool m_streamingMode{false};
    bool m_streamingFirstPtsVideo{true};
    bool m_streamingFirstFrameAudio{true};
    std::chrono::duration<double> m_firstOriginPTS{0.f};
    std::chrono::duration<double> m_streamingFramePTSAudio;
#ifdef MEASUREMENT_LOG
    long long m_previous_tp{ 0 };
#endif
#endif // STREAMING

    std::chrono::duration<double> m_hapticInitTime{0.f};

public:
    ~DecoderInterface() override;
    void onConfigure(const std::string &configFile) override;
    void setSharedOpenGLContext(HANDLE hwContext) override;
    void onStartEvent(unsigned mediaId) override;
    void onStopEvent() override;
    void onErrorEvent(unsigned int errorLevel, unsigned int errorId)
    {
        if (m_onErrorEventCallback)
        {
            m_onErrorEventCallback(errorLevel, errorId);
        }
    }
    void onChunkEvent(Chunk &&chunk) override;
    void onMediaRequest(unsigned mediaId) override;

    double getDecoderFPS() override
    {        
        double val = -1.0;
        std::chrono::duration<double> dt;
        
        m_queueMutex.lock();
        if (!m_queueDTpkt.empty())
        {
            dt = m_queueDTpkt.front();
            m_queueDTpkt.pop();
            val = 0.001 * std::chrono::duration_cast<std::chrono::milliseconds>(dt).count();
        }
        m_queueMutex.unlock();
        return val;
    }

    void flushFPSMeasures() override
    {
        m_queueMutex.lock();
        while (!m_queueDTpkt.empty())
        {
            m_queueDTpkt.pop();
        }
        m_queueMutex.unlock();
    }

    int getAtlasFrameHeight() override { return m_atlasFrameHeight; }
    int getAtlasFrameWidth() override { return m_atlasFrameWidth; }


private:
    void onStart() override;
    void initialize() override;
    void idle() override;
    void finalize() override;

#if defined DASH_STREAMING || defined UVG_RTP_STREAMING
public:
    void linkClient(Client::Interface *client); //, std::chrono::duration<double> frametime);
    bool decoderHasRestarted();
#endif                                          // STREAMING

private:
    
    void allocateAudioDecoder(std::string avcodec_name);
    void stopAudioDecoder();

    void allocateVideoDecoders(std::string avcodec_name);
    void stopVideoDecoders();

    void stopDecoders();

    void allocateHapticDecoder();
    void stopHapticDecoder();
};
