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

#include <cmath>

namespace iloj::misc
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename CLOCK>
Timer<CLOCK>::Timer(): m_state(StateId::STOPPED)
{
}

template<typename CLOCK>
auto Timer<CLOCK>::start() -> time_point
{
    m_state.lock();

    if (m_state.unsafe_get() == StateId::STOPPED)
    {
        m_t0 = CLOCK::now();
        m_state.unsafe_set(StateId::RUNNING);
    }

    m_state.unlock();

    return m_t0;
}

template<typename CLOCK>
void Timer<CLOCK>::stop()
{
    m_state.lock();
    if (m_state.unsafe_get() == StateId::RUNNING)
    {
        m_state.unsafe_set(StateId::STOPPED);
    }
    m_state.unlock();
}

template<typename CLOCK>
auto Timer<CLOCK>::restart() -> time_point
{
    m_state.lock();

    m_t0 = CLOCK::now();
    m_state.unsafe_set(StateId::RUNNING);

    m_state.unlock();

    return m_t0;
}

template<typename CLOCK>
auto Timer<CLOCK>::elapsed() -> duration
{
    return std::chrono::duration_cast<duration>(CLOCK::now() - m_t0);
}

template<typename CLOCK>
void Timer<CLOCK>::wait_until(time_point t)
{
    auto t0 = CLOCK::now();

    if (t0 < t)
    {
        std::this_thread::sleep_for(t - t0);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename CLOCK>
void Tempo<CLOCK>::start()
{
    m_state.lock();

    if (m_state.unsafe_get() == StateId::STOPPED)
    {
        m_firstTimePoint = CLOCK::now();
        m_nextTimePoint = m_firstTimePoint + m_period;
        m_state.unsafe_set(StateId::RUNNING);
    }

    m_state.unlock();
}

template<typename CLOCK>
void Tempo<CLOCK>::stop()
{
    m_state.lock();

    if (m_state.unsafe_get() == StateId::RUNNING)
    {
        m_state.unsafe_set(StateId::STOPPED);
    }

    m_state.unlock();
}

template<typename CLOCK>
void Tempo<CLOCK>::wait()
{
    if ((m_state.get() == StateId::RUNNING) && (m_period.count() != 0))
    {
        auto t0 = CLOCK::now();

        if (t0 < m_nextTimePoint)
        {
            std::this_thread::sleep_for(m_nextTimePoint - t0);
        }

        m_nextTimePoint += m_period;
    }
}

template<typename CLOCK>
auto Tempo<CLOCK>::elapsed() const -> duration
{
    return std::chrono::duration_cast<duration>(CLOCK::now() - m_firstTimePoint);
}

} // namespace iloj::misc
