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

#include <interface/audio.h>

class AudioInterface: public Audio::Interface
{
private:
    using OnStartEvent_Callback = void();
    using OnCameraMotion_Callback = void(float, float, float, float, float, float, float);
    using OnSampleEvent_Callback = void(unsigned, unsigned, unsigned, unsigned, const void *, unsigned);
    using OnPauseEvent_Callback = void(bool);
    using OnStopEvent_Callback = void();

private:
    OnStartEvent_Callback *OnStartEvent = nullptr;
    OnCameraMotion_Callback *OnCameraMotion = nullptr;
    OnSampleEvent_Callback *OnSampleEvent = nullptr;
    OnPauseEvent_Callback *OnPauseEvent = nullptr;
    OnStartEvent_Callback *OnStopEvent = nullptr;

public:
    AudioInterface();
    AudioInterface(const AudioInterface &) = delete;
    AudioInterface(AudioInterface &&other) = default;
    auto operator=(const AudioInterface &) -> AudioInterface & = delete;
    auto operator=(AudioInterface &&other) -> AudioInterface & = default;
    void onConfigure(const std::string & /* configFile */) override {}
    void onStartEvent() override;
    void onCameraMotion(const std::array<float, 3> &position, const std::array<float, 4> &quaternion) override;
    void onSampleEvent(const AudioPacket &pkt) override;
    void onPauseEvent(bool b) override;
    void onStopEvent() override;
};
