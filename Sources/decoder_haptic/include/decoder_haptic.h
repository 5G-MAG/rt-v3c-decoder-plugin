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

#include <string>
#include <types.h>

#include <common/misc/types_haptic.h>

class DecoderHaptic
{
private:
    std::chrono::duration<double> m_InitTime{0.0f};

public:
    DecoderHaptic() = default;
    ~DecoderHaptic() = default;
    DecoderHaptic(std::chrono::duration<double> initTime);
    DecoderHaptic(const DecoderHaptic &) = delete;
    DecoderHaptic(DecoderHaptic &&) noexcept = default;
    auto operator=(const DecoderHaptic &) -> DecoderHaptic & = delete;
    auto operator=(DecoderHaptic &&) noexcept -> DecoderHaptic & = default;

    void decode(std::string s, HapticInput& hapticInput);
};
