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
#include <common/misc/types_haptic.h>
#include <string>

namespace Haptic
{
class Interface
{
public:
    Interface() = default;
    virtual ~Interface() = default;
    Interface(const Interface &) = delete;
    Interface(Interface &&other) noexcept = default;
    auto operator=(const Interface &) -> Interface & = delete;
    auto operator=(Interface &&) noexcept -> Interface & = default;
    virtual void onConfigure(const std::string &configFile) = 0;
    virtual void onStartEvent() = 0;
    virtual void onSampleEvent(const HapticPacket &pkt) = 0;
    virtual void onRenderEvent() = 0;
    virtual void onPauseEvent(bool b) = 0;
    virtual void onStopEvent() = 0;
};

} // namespace Haptic