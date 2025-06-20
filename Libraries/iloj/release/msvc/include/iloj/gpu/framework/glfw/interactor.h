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

#include "joystick.h"

namespace iloj::gpu::framework::glfw
{
namespace Interactor
{
/////////////////////////////////////////////////////////////////
class Base
{
public:
    virtual void postMouseButtonPress(gpu::Interactor::MouseButton button, int x, int y) = 0;
    virtual void postMouseButtonRelease(gpu::Interactor::MouseButton button, int x, int y) = 0;
    virtual void postMouseMove(int x, int y) = 0;
    virtual void postKeyPress(unsigned short key) = 0;
    virtual void postKeyRelease(unsigned short key) = 0;
    virtual void postScroll(int dx, int dy) = 0;
    virtual void postJoystickEvent(const iloj::gpu::Interactor::Joystick &joystick) = 0;
    virtual void postCustomEvent(unsigned id, void *data) = 0;
    virtual void postDragAndDrop(const std::vector<std::string> &droppedItems) = 0;
};

/////////////////////////////////////////////////////////////////
template<typename M>
class Generic: public Base, public M
{
public:
    template<typename... ARGS>
    Generic(ARGS &&... args): M(std::forward<ARGS>(args)...)
    {
    }
    void postMouseButtonPress(gpu::Interactor::MouseButton button, int x, int y) override
    {
        M::onMouseButtonPress(button, x, y);
    }
    void postMouseButtonRelease(gpu::Interactor::MouseButton button, int x, int y) override
    {
        M::onMouseButtonRelease(button, x, y);
    }
    void postMouseMove(int x, int y) override { M::onMouseMove(x, y); }
    void postKeyPress(unsigned short key) override { M::onKeyPress(key); }
    void postKeyRelease(unsigned short key) override { M::onKeyRelease(key); }
    void postScroll(int dx, int dy) override { M::onScroll(dx, dy); }
    void postJoystickEvent(const iloj::gpu::Interactor::Joystick &joystick) override { M::onJoystickEvent(joystick); }
    void postCustomEvent(unsigned id, void *data) override { M::onCustomEvent(id, data); }
    void postDragAndDrop(const std::vector<std::string> &droppedItems) override { M::onDragAndDrop(droppedItems); }
};
} // namespace Interactor

template<typename M, typename... ARGS>
auto make_interactor(ARGS &&... args) -> std::unique_ptr<Interactor::Generic<M>>
{
    return std::make_unique<Interactor::Generic<M>>(std::forward<ARGS>(args)...);
}
} // namespace iloj::gpu::framework::glfw
