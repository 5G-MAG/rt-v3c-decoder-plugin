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

#include "event.h"
#include "joystick.h"
#include <iloj/misc/memory.h>
#include <iloj/misc/thread.h>
#include <queue>
#include <set>

namespace iloj::gpu::framework::glfw
{
namespace Window
{
class OnScreen;
}

class Application
{
    friend class Window::OnScreen;

private:
    static iloj::misc::Reference<Application> g_current;
    static bool g_verticalSynchronization;
    static Window::OnScreen *g_focusedWindow;

private:
    bool m_flag = false;
    std::set<Window::OnScreen *> m_renderers;
    iloj::misc::pVar<std::queue<std::pair<Window::OnScreen &, Event>>> m_pendingEvent;
    iloj::misc::Semaphore m_semEvent;
    unsigned m_nbRenderer = 0;
    bool m_vsync = true;
    std::vector<Joystick> m_joystick;

public:
    Application();
    ~Application();
    Application(const Application &) = delete;
    Application(Application &&) = delete;
    auto operator=(const Application &) -> Application & = delete;
    auto operator=(Application &&) -> Application & = delete;
    auto exec() -> int;
    void quit();
    void postEvent(Window::OnScreen &w, Event e);
    static auto current() -> Application & { return g_current.get(); }
    static auto getNumberOfMonitors() -> int;
    static void getMonitorLayout(int id, int &x, int &y, int &w, int &h);
    static auto getMonitorResolution(int id = 0) -> float;
    static void disableVerticalSynchronization();

protected:
    auto isVerticalSynchronizationEnabled() const -> bool { return m_vsync; }
    void registerRenderer(Window::OnScreen *w) { m_renderers.insert(w); }
    void unregisterRenderer(Window::OnScreen *w) { m_renderers.erase(w); }
    void processCustomEvents();
    static void errorCallback(int code, const char *description);
};
} // namespace iloj::gpu::framework::glfw
