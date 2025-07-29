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

#include <audio/audio.h>
#include <iloj/misc/dll.h>
#include <iloj/misc/logger.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
AudioInterface::AudioInterface()
{
#if defined _WIN64
    static const std::string pluginName = "AudioPlugin_V3CImmersiveDecoderAudio.dll";
#elif defined __linux__
    static const std::string pluginName = "libAudioPlugin_V3CImmersiveDecoderAudio.so";
#endif

    LoadProc(pluginName, OnStartEvent);
    LoadProc(pluginName, OnCameraMotion);
    LoadProc(pluginName, OnSampleEvent);
    LoadProc(pluginName, OnPauseEvent);
    LoadProc(pluginName, OnStopEvent);
}

void AudioInterface::onStartEvent()
{
    LOG_INFO("AudioInterface::onStartEvent");

    if (OnStartEvent)
    {
        OnStartEvent();
    }
}

void AudioInterface::onCameraMotion(const std::array<float, 3> &position, const std::array<float, 4> &quaternion)
{
    if (OnCameraMotion)
    {
        OnCameraMotion(
            position[0], position[1], position[2], quaternion[0], quaternion[1], quaternion[2], quaternion[3]);
    }
}

void AudioInterface::onSampleEvent(const AudioPacket &pkt)
{
#if 0
    LOG_INFO("AudioInterface::onSampleEvent");
    int ii = 0;
    std::string dataStr;
    for (ii=0;ii<10;ii++)
    {
        std::string mystr = std::to_string(pkt->m_buffer.data()[ii]);
        dataStr = dataStr + " " + mystr;
    }
    LOG_INFO("  - ", dataStr);
#endif
    if (OnSampleEvent)
    {
        OnSampleEvent(static_cast<unsigned>(pkt->getFormat()),
                      static_cast<unsigned>(pkt->getPacking()),
                      pkt->getNumberOfChannels(),
                      pkt->getRate(),
                      pkt->m_buffer.data(),
                      static_cast<unsigned>(pkt->m_buffer.size()));
    }
}

void AudioInterface::onPauseEvent(bool b)
{
    if (OnPauseEvent)
    {
        OnPauseEvent(b);

        if (b)
        {
            LOG_INFO("AudioInterface::onPauseEvent");
        }
        else
        {
            LOG_INFO("AudioInterface::onResumeEvent");
        }
    }
}

void AudioInterface::onStopEvent()
{
    if (OnStopEvent)
    {
        OnStopEvent();
    }

    LOG_INFO("AudioInterface::onStopEvent");
}
