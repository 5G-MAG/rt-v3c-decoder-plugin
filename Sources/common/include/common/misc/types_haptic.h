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

#include <iloj/gpu/types.h>
#include <iloj/media/descriptor.h>
#include <iloj/misc/packet.h>

class HapticDescriptor
{
public:
    using container_type = std::vector<std::uint8_t>;

private:
    int m_channelId{0};
    std::chrono::duration<double> m_startTimestamp{};
    std::chrono::duration<double> m_endTimestamp{};
    float m_startIntensity{1.0f};
    float m_endIntensity{1.0f};
    std::function<void(HapticDescriptor &)> m_onFrameCallback;

public:
    container_type m_buffer;

public:
    HapticDescriptor() = default;
    ~HapticDescriptor() = default;
    HapticDescriptor(const HapticDescriptor &other) = default;
    HapticDescriptor(HapticDescriptor && /*other*/) noexcept {}
    auto operator=(const HapticDescriptor &other) -> HapticDescriptor & = default;
    auto operator=(HapticDescriptor &&other) noexcept -> HapticDescriptor &
    {
        return *this;
    }
    [[nodiscard]] auto getChannelId() const -> int { return m_channelId; }
    [[nodiscard]] auto getStartTimeStamp() const -> const std::chrono::duration<double> & { return m_startTimestamp; }
    [[nodiscard]] auto getEndTimeStamp() const -> const std::chrono::duration<double> & { return m_endTimestamp; }
    [[nodiscard]] auto getStartIntensity() const -> const float { return m_startIntensity; }
    [[nodiscard]] auto getEndIntensity() const -> const float { return m_endIntensity; }

    void setChannelId(const int v) { m_channelId = v; }
    void setStartTimeStamp(const std::chrono::duration<double> &v) { m_startTimestamp = v; }
    void setEndTimeStamp(const std::chrono::duration<double> &v) { m_endTimestamp = v; }
    void setStartIntensity(const float v) { m_startIntensity = v; }
    void setEndIntensity(const float v) { m_endIntensity = v; }

    auto setOnFrameCallback(const std::function<void(HapticDescriptor &)> &callback) { m_onFrameCallback = callback; }
};

using HapticPacket = iloj::misc::Packet<HapticDescriptor>;
using HapticInput = iloj::misc::Input<HapticDescriptor>;
using HapticOutput = iloj::misc::Output<HapticDescriptor>;