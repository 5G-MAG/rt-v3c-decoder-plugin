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
#include <iloj/misc/json.h>
#include <iloj/misc/thread.h>
#include <iloj/misc/packet.h>
#include <common/misc/types_haptic.h>

class HapticDecoder
{
private:
    HapticInput *m_hapticInput;

public:
    HapticDecoder() = default;
    HapticDecoder(const HapticDecoder &) = delete;
    HapticDecoder(HapticDecoder &&) = delete;
    ~HapticDecoder() = default;
    auto operator=(const HapticDecoder &) -> HapticDecoder & = delete;
    auto operator=(HapticDecoder &&) -> HapticDecoder & = delete;
    void setHapticInput(HapticInput &i) { m_hapticInput = &i; }
    auto getHapticInput() -> HapticInput & { return *m_hapticInput; }
};