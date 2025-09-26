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

class Synthesizer
{
public:
    Synthesizer() = default;
    ~Synthesizer() = default;
    Synthesizer(const std::string &configFile, unsigned synthesizerId);
    Synthesizer(const Synthesizer &) = delete;
    Synthesizer(Synthesizer &&) noexcept = default;
    auto operator=(const Synthesizer &) -> Synthesizer & = delete;
    auto operator=(Synthesizer &&) noexcept -> Synthesizer & = default;

    void synthesize(const int channelId, const long duration, const float startIntensity, const float endIntensity);
};
