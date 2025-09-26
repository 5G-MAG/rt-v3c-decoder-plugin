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

#include "audio.h"
#include "video.h"
#include "haptic.h"

namespace Scheduler
{
class Interface
{
protected:
    Audio::Interface *m_audioInterface = nullptr;
    Video::Interface *m_videoInterface = nullptr;
    Haptic::Interface *m_hapticInterface = nullptr;

public:
    Interface(){};
    virtual ~Interface() {}
    Interface(const Interface &other) = delete;
    Interface(Interface &&other) noexcept = default;
    auto operator=(const Interface &) -> Interface & = delete;
    auto operator=(Interface &&other) noexcept -> Interface & = default;
    void setAudioInterface(Audio::Interface *audioInterface) { m_audioInterface = audioInterface; }
    void setVideoInterface(Video::Interface *videoInterface) { m_videoInterface = videoInterface; }
    void setHapticInterface(Haptic::Interface* hapticInterface) { m_hapticInterface = hapticInterface; }

    virtual auto getAudioInput() -> AudioInput & = 0;
    virtual auto getVideoInput() -> DecodedVideoInput & = 0;
    virtual auto getHapticInput() -> HapticInput & = 0;

    virtual void onConfigure(const std::string &configFile) = 0;
    virtual void onStartEvent() = 0;
    virtual void onStopEvent() = 0;
};

} // namespace Scheduler
