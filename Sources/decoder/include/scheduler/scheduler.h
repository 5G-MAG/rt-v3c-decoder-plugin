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

#include <chrono>
#include <iloj/media/descriptor.h>
#include <iloj/misc/packet.h>
#include <interface/audio.h>
#include <interface/scheduler.h>
#include <interface/haptic.h>

class SchedulerInterface: public Scheduler::Interface
{
private:

    class MasterClock
    {
    private:
        bool m_forceDecodersSynchro = true;   
        std::chrono::duration<double> m_offset{0};
        std::chrono::duration<double> m_initTime{0};

    public:
        std::chrono::duration<double> now();
        void updateOffset(std::chrono::milliseconds offset);
        void setForceDecodersSynchro(bool force_synchro) { m_forceDecodersSynchro = force_synchro; }
        void reset()
        {
            m_offset = (std::chrono::duration<double>) 0;
            m_initTime = now();
        }
        std::chrono::duration<double> getTimeRelative(std::chrono::duration<double> time) { return time - m_initTime;}
        std::chrono::duration<double> getOffset() { return m_offset; }
    };


    class AudioScheduler: public iloj::misc::Service
    {
    private:
        SchedulerInterface::MasterClock *m_masterClock;
        Audio::Interface *m_audioInterface = nullptr;
        std::chrono::milliseconds m_latency{0};
        AudioInput m_input;

    public:
        AudioScheduler(MasterClock *clock) { m_masterClock = clock; }

        void setInterface(Audio::Interface *audioInterface) { m_audioInterface = audioInterface; }
        void setLatency(std::chrono::milliseconds latency) { m_latency = latency; }
        auto getInput() -> AudioInput & { return m_input; }

    private:
        void initialize() override;
        void idle() override;
        void finalize() override;
    };

    class VideoScheduler: public iloj::misc::Service
    {
    private:
        SchedulerInterface::MasterClock *m_masterClock;

        Video::Interface *m_videoInterface = nullptr;
        std::chrono::milliseconds m_jitter{5};
        DecodedVideoInput m_input;

        
        //std::chrono::milliseconds m_offset{0};

    public:
        VideoScheduler(MasterClock *clock) { m_masterClock = clock; }
        
        void setInterface(Video::Interface *videoInterface) { m_videoInterface = videoInterface; }
        void setJitter(std::chrono::milliseconds jitter) { m_jitter = jitter; }
        
        auto getInput() -> DecodedVideoInput & { return m_input; }

    private:
        void initialize() override;
        void idle() override;
        void finalize() override;
    };

    class HapticScheduler: public iloj::misc::Service
    {
    private:
        SchedulerInterface::MasterClock *m_masterClock;
        Haptic::Interface *m_hapticInterface = nullptr;
        std::chrono::milliseconds m_latency{0};
        HapticInput m_input;
        
    public:
        HapticScheduler(MasterClock *clock) { m_masterClock = clock; }

        void setInterface(Haptic::Interface *hapticInterface) { m_hapticInterface = hapticInterface; }
        void setLatency(std::chrono::milliseconds latency) { m_latency = latency; }
        auto getInput() -> HapticInput & { return m_input; }

    private:
        void initialize() override;
        void idle() override;
        void finalize() override;
    };

private:

    MasterClock m_masterClock;

    AudioScheduler m_audioScheduler{&m_masterClock};
    VideoScheduler m_videoScheduler{&m_masterClock};
    HapticScheduler m_hapticScheduler{&m_masterClock};

public:
    void onConfigure(const std::string &configFile) override;
    void onStartEvent() override;
    void onStopEvent() override;
    auto getAudioInput() -> AudioInput & override { return m_audioScheduler.getInput(); }
    auto getVideoInput() -> DecodedVideoInput & override { return m_videoScheduler.getInput(); }
    auto getHapticInput() -> HapticInput & override { return m_hapticScheduler.getInput(); }
};
