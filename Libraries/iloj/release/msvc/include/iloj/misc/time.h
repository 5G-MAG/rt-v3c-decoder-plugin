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

#include "thread.h"
#include <chrono>

namespace iloj::misc
{
template<typename CLOCK>
class Timer
{
public:
    enum class StateId
    {
        STOPPED = 0,
        RUNNING
    };

    using time_point = std::chrono::time_point<CLOCK>;
    using duration = std::chrono::milliseconds;

private:
    time_point m_t0;
    pVar<StateId, SpinLock> m_state;

public:
    Timer();
    auto start() -> time_point;
    void stop();
    auto restart() -> time_point;
    auto elapsed() -> duration;
    auto running() -> bool { return (m_state.get() != StateId::STOPPED); }
    void wait_for(duration ms) { std::this_thread::sleep_for(ms); }
    void wait_until(time_point t);
};

template<typename CLOCK>
class Tempo
{
public:
    enum class StateId
    {
        STOPPED = 0,
        RUNNING
    };

    using time_point = std::chrono::time_point<CLOCK>;
    using duration = std::chrono::milliseconds;

private:
    duration m_period{};
    time_point m_firstTimePoint, m_nextTimePoint;
    pVar<StateId, SpinLock> m_state;

public:
    explicit Tempo(const duration &period): m_period{period} {}
    void setPeriod(const duration &period) { m_period = period; }
    [[nodiscard]] auto getPeriod() const -> const duration & { return m_period; }
    void start();
    void stop();
    void wait();
    [[nodiscard]] auto elapsed() const -> duration;
};

} // namespace iloj::misc

#include "time.hpp"
