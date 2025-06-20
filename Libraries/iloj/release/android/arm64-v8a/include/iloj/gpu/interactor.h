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

#include <iloj/misc/memory.h>
#include <iloj/misc/thread.h>

namespace iloj::gpu
{
class Renderer;

class Interactor
{
public:
    enum class MouseButton
    {
        None = 0,
        Left = 1,
        Right = 2,
        Middle = 4
    };

    class Joystick
    {
    public:
        struct Hat
        {
            enum Id
            {
                CENTERED = 0,
                UP = 1,
                RIGHT = 2,
                DOWN = 4,
                LEFT = 8
            };
        };

    public:
        Joystick() = default;
        virtual ~Joystick() = default;
        Joystick(const Joystick &other) = default;
        Joystick(Joystick &&other) = default;
        auto operator=(const Joystick &other) -> Joystick & = default;
        auto operator=(Joystick &&other) -> Joystick & = default;
        [[nodiscard]] virtual auto getName() const -> const std::string & = 0;
        [[nodiscard]] virtual auto getNumberOfAxis() const -> std::size_t = 0;
        [[nodiscard]] virtual auto getAxisState(std::size_t axisId = 0) const -> float = 0;
        [[nodiscard]] virtual auto getNumberOfButtons() const -> std::size_t = 0;
        [[nodiscard]] virtual auto getButtonState(std::size_t buttonId = 0) const -> std::uint8_t = 0;
        [[nodiscard]] virtual auto getNumberOfHats() const -> std::size_t = 0;
        [[nodiscard]] virtual auto getHatState(std::size_t hatId = 0) const -> std::uint8_t = 0;
    };

protected:
    iloj::misc::Reference<Renderer> m_renderer;
    iloj::misc::SpinLock m_locker;

public:
    Interactor() = default;
    virtual ~Interactor() = default;
    Interactor(const Interactor &) = delete;
    Interactor(Interactor &&) = delete;
    auto operator=(const Interactor &) -> Interactor & = delete;
    auto operator=(Interactor &&) -> Interactor & = delete;
    void attach(Renderer &renderer)
    {
        m_renderer = renderer;
        onAttachment(renderer);
    }
    auto getRenderer() -> Renderer & { return m_renderer.get(); }
    void lock() { m_locker.lock(); }
    void unlock() { m_locker.unlock(); }

public:
    virtual void onAttachment(Renderer & /*renderer*/) {}
    virtual void onMouseButtonPress(MouseButton /*button*/, int /*x*/, int /*y*/) {}
    virtual void onMouseButtonRelease(MouseButton /*button*/, int /*x*/, int /*y*/) {}
    virtual void onMouseMove(int /*x*/, int /*y*/) {}
    virtual void onKeyPress(unsigned short /*key*/) {}
    virtual void onKeyRelease(unsigned short /*key*/) {}
    virtual void onScroll(int /*dx*/, int /*dy*/) {}
    virtual void onJoystickEvent(const Joystick & /*joystick*/) {}
    virtual void onCustomEvent(unsigned /*unused*/, void * /*unused*/) {}
    virtual void onDragAndDrop(const std::vector<std::string> & /*draggedItems*/) {}
};
} // namespace iloj::gpu
