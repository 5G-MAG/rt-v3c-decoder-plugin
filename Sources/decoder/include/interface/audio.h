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

#include <common/misc/types.h>

namespace Audio
{
class Interface
{
public:
    Interface() = default;
    virtual ~Interface() {}
    Interface(const Interface &other) = delete;
    Interface(Interface &&other) noexcept = default;
    auto operator=(const Interface &) -> Interface & = delete;
    auto operator=(Interface &&other) noexcept -> Interface & = default;
    virtual void onConfigure(const std::string &configFile) = 0;
    virtual void onStartEvent() = 0;
    virtual void onCameraMotion(const std::array<float, 3> &position, const std::array<float, 4> &quaternion) = 0;
    virtual void onSampleEvent(const AudioPacket &pkt) = 0;
    virtual void onPauseEvent(bool b) = 0;
    virtual void onStopEvent() = 0;
};

} // namespace Audio
