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
#include <iloj/misc/time.h>

namespace iloj::gpu
{
class Renderer;

class Scheduler: public iloj::misc::Service
{
protected:
    Renderer &m_renderer;
    iloj::misc::Tempo<std::chrono::high_resolution_clock> m_tempo;
    bool m_lazyMode = false;
    bool m_forcedMode = false;
    std::atomic_flag m_flag{};

public:
    explicit Scheduler(Renderer &renderer): m_renderer(renderer), m_tempo(getDefaultPeriod()) { m_flag.clear(); }
    void requestRefresh() { m_flag.clear(); }
    void enableLazyMode(bool b) { m_lazyMode = b; }
    void enableForcedMode(bool b) { m_forcedMode = b; }
    auto isForcedModeEnabled() const -> bool { return m_forcedMode; }
    void setRefreshPeriod(const std::chrono::milliseconds &period) { m_tempo.setPeriod(period); }
    static auto getDefaultPeriod() -> const std::chrono::milliseconds &
    {
        static const std::chrono::milliseconds defaultPeriod{1};
        return defaultPeriod;
    }

protected:
    void initialize() override
    {
        m_flag.clear();
        m_tempo.start();
    }
    void idle() override;
    void finalize() override { m_tempo.stop(); }
};
} // namespace iloj::gpu
