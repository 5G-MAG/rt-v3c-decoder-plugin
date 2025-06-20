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

#include <iloj/misc/time.h>

namespace iloj::gpu
{
class Benchmarker
{
private:
    iloj::misc::Timer<std::chrono::high_resolution_clock> m_timer;
    std::size_t m_windowLength{300}, m_nbFrames{}, m_nbFrames1{}, m_nbFrames2{};
    std::chrono::milliseconds::rep m_checkPoint1{}, m_checkPoint2{};
    double m_fps = 0.;

public:
    void setWindowLength(unsigned v) { m_windowLength = v; }
    void start()
    {
        m_fps = 0.;
        m_nbFrames = 0;
        m_nbFrames1 = m_windowLength / 2;
        m_nbFrames2 = 0;
        m_checkPoint1 = 0;
        m_checkPoint2 = 0;
        m_timer.restart();
    }
    void signal()
    {
        auto checkPoint = m_timer.elapsed().count();

        if (m_windowLength < ++m_nbFrames1)
        {
            m_nbFrames1 = 0;
            m_checkPoint1 = checkPoint;
        }

        if (m_windowLength < ++m_nbFrames2)
        {
            m_nbFrames2 = 0;
            m_checkPoint2 = checkPoint;
        }

        if (++m_nbFrames < m_windowLength)
        {
            m_fps = 1000. * m_nbFrames / checkPoint;
        }
        else if (m_nbFrames2 < m_nbFrames1)
        {
            m_fps = 1000. * m_nbFrames1 / (checkPoint - static_cast<double>(m_checkPoint1));
        }
        else
        {
            m_fps = 1000. * m_nbFrames2 / (checkPoint - static_cast<double>(m_checkPoint2));
        }
    }
    auto frames() const -> unsigned { return m_nbFrames; }
    auto fps() const -> double { return m_fps; }
    auto time() -> std::chrono::milliseconds { return m_timer.elapsed(); }
};
} // namespace iloj::gpu
