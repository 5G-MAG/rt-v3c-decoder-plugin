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
#include <interface/haptic.h>

class HapticInterface: public Haptic::Interface
{
private:
    using OnStartEvent_Callback = void();
    using OnSampleEvent_Callback = void(const int channelId, const long duration, const float startIntensity, const float endIntensity);
    using OnRenderEvent_Callback = void(const std::string &);
    using OnPauseEvent_Callback = void(bool);
    using OnStopEvent_Callback = void();

private:
    OnStartEvent_Callback *OnStartEvent = nullptr;
    OnSampleEvent_Callback *OnSampleEvent = nullptr;
    OnRenderEvent_Callback *OnRenderEvent = nullptr;
    OnPauseEvent_Callback *OnPauseEvent = nullptr;
    OnStartEvent_Callback *OnStopEvent = nullptr;

public:
    HapticInterface();
    HapticInterface(const HapticInterface &) = delete;
    HapticInterface(HapticInterface &&other) = default;
    auto operator=(const HapticInterface &) -> HapticInterface & = delete;
    auto operator=(HapticInterface &&other) -> HapticInterface & = default;
    void onConfigure(const std::string & /* configFile */) override {}
    void onStartEvent() override;
    void onSampleEvent(const HapticPacket &pkt) override;
    void onRenderEvent() override;
    void onPauseEvent(bool b) override;
    void onStopEvent() override;
};