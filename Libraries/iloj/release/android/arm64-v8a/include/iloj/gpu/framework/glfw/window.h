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

#include "interactor.h"
#include <iloj/gpu/processor.h>
#include <iloj/gpu/renderer.h>
#include <map>

struct GLFWwindow;

namespace iloj::gpu::framework::glfw
{
namespace Window
{
class OnScreen: public gpu::Renderer
{
public:
    enum class Mode
    {
        Windowed,
        FullScreen
    };
    enum class KeyAction
    {
        None,
        Pressed,
        Released
    };

private:
    GLFWwindow *m_window = nullptr;
    int m_currentKeyAction = -1, m_currentKeyValue = -1;
    std::map<int, int> m_keyMap;
    std::function<void()> m_onCloseEvent, m_onMoveEvent, m_onFocusEvent;

public:
    OnScreen(const std::string &title = "iloj::gpu::framework::glfw",
             int w = 640,
             int h = 480,
             Mode mode = Mode::Windowed,
             bool resizable = true,
             GLFWwindow *shared = nullptr);
    ~OnScreen() override;
    OnScreen(const OnScreen &) = delete;
    OnScreen(OnScreen &&) = delete;
    auto operator=(const OnScreen &) -> OnScreen & = delete;
    auto operator=(OnScreen &&) -> OnScreen & = delete;
    auto getWindow() const -> GLFWwindow * { return m_window; }
    void show();
    void close();
    void focus();
    void iconify();
    void restore();
    void maximize();
    void resize(int w, int h);
    auto size() const -> std::array<int, 2>;
    void move(int left, int top);
    void center(int monitorId = 0);
    auto pos() const -> std::array<int, 2>;
    void setTitle(const std::string &s);
    void setIcon(int w, int h, unsigned char *pixels); // NOLINT
    void setOnCloseEventFunction(std::function<void()> f) { m_onCloseEvent = std::move(f); }
    void setOnMoveEventFunction(std::function<void()> f) { m_onMoveEvent = std::move(f); }
    void setOnFocusEventFunction(std::function<void()> f) { m_onFocusEvent = std::move(f); }
    template<typename I, typename... ARGS>
    void addInteractor(ARGS &&... args)
    {
        gpu::Renderer::addInteractor(std::make_unique<Interactor::Generic<I>>(std::forward<ARGS>(args)...));
    }
    auto getNativeHandle() const -> GLvoid * override;

protected:
    void setupVerticalSynchronization();
    void acquireGLContext() override;
    void releaseGLContext() override;
    void swapGLBuffers() override;

protected:
    virtual void onCloseEvent()
    {
        if (m_onCloseEvent)
        {
            m_onCloseEvent();
        }
    }
    virtual void onMoveEvent(int /*unused*/, int /*unused*/)
    {
        if (m_onMoveEvent)
        {
            m_onMoveEvent();
        }
    }
    virtual void onFocusEvent()
    {
        if (m_onFocusEvent)
        {
            m_onFocusEvent();
        }
    }

protected:
    static void moveCallback(GLFWwindow *win, int x, int y);
    static void resizeCallback(GLFWwindow *win, int w, int h);
    static void iconifyCallback(GLFWwindow *win, int value);
    static void closeCallback(GLFWwindow *win);
    static void focusCallback(GLFWwindow *win, int value);
    static void keyCallback(GLFWwindow *win, int key, int scancode, int action, int mods);
    static void charCallback(GLFWwindow *win, unsigned code);
    static void mouseCallback(GLFWwindow *win, int button, int action, int mods);
    static void cursorCallback(GLFWwindow *win, double xpos, double ypos);
    static void scrollCallback(GLFWwindow *win, double xoffset, double yoffset);
    static void dragAndDropCallback(GLFWwindow *win, int count, const char **paths);
};

class OffScreen: public gpu::Processor
{
protected:
    GLFWwindow *m_window = nullptr;

public:
    OffScreen(GLFWwindow *shared = nullptr);
    ~OffScreen() override;
    OffScreen(const OffScreen &) = delete;
    OffScreen(OffScreen &&) = delete;
    auto operator=(const OffScreen &) -> OffScreen & = delete;
    auto operator=(OffScreen &&) -> OffScreen & = delete;
    [[nodiscard]] auto getWindow() const -> GLFWwindow * { return m_window; }
    [[nodiscard]] auto getNativeHandle() const -> GLvoid * override;

protected:
    void acquireGLContext() override;
    void releaseGLContext() override;
};
} // namespace Window
} // namespace iloj::gpu::framework::glfw
