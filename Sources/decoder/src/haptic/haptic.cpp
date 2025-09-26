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

#include <haptic/haptic.h>
#include <iloj/misc/dll.h>
#include <iloj/misc/logger.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
HapticInterface::HapticInterface()
{
#if defined _WIN64
    static const std::string pluginName = "V3CImmersiveSynthesizerHaptic.dll";
#elif defined __linux__
    static const std::string pluginName = "libV3CImmersiveSynthesizerHaptic.so";
#endif

    LoadProc(pluginName, OnStartEvent);
    LoadProc(pluginName, OnSampleEvent);
    LoadProc(pluginName, OnRenderEvent);
    LoadProc(pluginName, OnPauseEvent);
    LoadProc(pluginName, OnStopEvent);
}

void HapticInterface::onStartEvent()
{
    LOG_INFO("HapticInterface::onStartEvent");

    if (OnStartEvent)
    {
        OnStartEvent();
    }
}

void HapticInterface::onSampleEvent(const HapticPacket &pkt)
{
    if (OnSampleEvent)
    {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(pkt.getContent().getEndTimeStamp() -
                                                                        pkt.getContent().getStartTimeStamp());

        OnSampleEvent(pkt.getContent().getChannelId(),
                      duration.count(),
                      pkt.getContent().getStartIntensity(),
                      pkt.getContent().getEndIntensity());
    }
}

void HapticInterface::onRenderEvent()
{
    if (OnRenderEvent)
    {
        OnRenderEvent("");
    }
}

void HapticInterface::onPauseEvent(bool b)
{
    if (OnPauseEvent)
    {
        OnPauseEvent(b);

        if (b)
        {
            LOG_INFO("HapticInterface::onPauseEvent");
        }
        else
        {
            LOG_INFO("HapticInterface::onResumeEvent");
        }
    }
}

void HapticInterface::onStopEvent()
{
    if (OnStopEvent)
    {
        OnStopEvent();
    }

    LOG_INFO("HapticInterface::onStopEvent");
}