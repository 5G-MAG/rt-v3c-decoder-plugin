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

#include <fstream>
#include <iloj/misc/filesystem.h>
#include <iloj/misc/logger.h>
#include <synthesizer.h>
#include <types.h>
#include <iostream>

static std::ofstream g_logStream;
static std::unique_ptr<Synthesizer> g_synthesizer;
static bool g_isPaused = false;
extern "C" INTERFACE_EXPORT void INTERFACE_API OnCreateEvent(const char *configFile, unsigned synthesizerId)
{
    using namespace iloj::misc;

    g_logStream.open(FileSystem::Path{configFile}.getParent().toString() +
                     "/V3CImmersiveSynthesizerHaptic.log");

    Logger::getInstance().setStream(g_logStream);
    Logger::getInstance().setLevel(Logger::Level::Info);
    Logger::getInstance().enableFlushOnAppend(true);

    g_synthesizer = std::make_unique<Synthesizer>(configFile, synthesizerId);
    std::cout << "OnCreateEvent" << std::endl;
    LOG_INFO("OnCreateEvent");
}

extern "C" INTERFACE_EXPORT void INTERFACE_API OnStartEvent(unsigned mediaId)
{
    std::cout << "OnStartEvent Media " << mediaId << std::endl;
    LOG_INFO("OnStartEvent Media ", mediaId);
}

extern "C" INTERFACE_EXPORT void INTERFACE_API OnPauseEvent(bool isPaused)
{
    LOG_INFO("OnPauseEvent ", isPaused);
    g_isPaused = isPaused;
}

typedef int(__stdcall *ANSWERCB)(int, long, float, float);
static ANSWERCB cb;
extern "C" INTERFACE_EXPORT void INTERFACE_API SetHapticCallback(ANSWERCB fp) { cb = fp; }

extern "C" INTERFACE_EXPORT void INTERFACE_API OnSampleEvent(const int channelId,
                                                             const long duration,
                                                             const float startIntensity,
                                                             const float endIntensity)
{
    g_synthesizer->synthesize(channelId, duration, startIntensity, endIntensity);
    if (cb && !g_isPaused)
    {
        cb(channelId, duration, startIntensity, endIntensity);
    }
}

extern "C" INTERFACE_EXPORT void INTERFACE_API OnStopEvent()
{
    std::cout << "OnStopEvent" << std::endl;
    LOG_INFO("OnStopEvent");
}

extern "C" INTERFACE_EXPORT void INTERFACE_API OnReleaseEvent()
{
    std::cout << "OnReleaseEvent" << std::endl;
    LOG_INFO("OnReleaseEvent");
}

extern "C" INTERFACE_EXPORT void INTERFACE_API OnDestroyEvent()
{
    std::cout << "OnDestroyEvent" << std::endl;
    LOG_INFO("OnDestroyEvent");
    g_logStream.close();
}
