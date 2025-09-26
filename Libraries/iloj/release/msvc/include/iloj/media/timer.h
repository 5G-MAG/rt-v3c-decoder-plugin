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

#include <iloj/misc/thread.h>
#include <chrono>

namespace iloj::media
{
class Timer
{
public:
    enum class StateId
    {
        Stopped = 0,
        Playing,
        Paused,
        Sought
    };

private:
    StateId m_state{StateId::Stopped};
    mutable iloj::misc::SpinLock m_pState;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_t0{};
    std::chrono::milliseconds::rep m_elapsed{};

public:
    void start();
    void restart();
    void stop();
    void pause();
    auto elapsed() const -> std::chrono::milliseconds::rep;
    void seek(double time);
    auto state() const -> StateId { return m_state; }
    auto wait_for(double ts) -> double;
};
} // namespace iloj::media
