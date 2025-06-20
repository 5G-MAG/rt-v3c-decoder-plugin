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

#include "widget.h"
#include <iloj/gpu/interactor.h>

namespace iloj::gui
{
class Interactor: public gpu::Interactor
{
private:
    Widget &m_rootWidget;

public:
    Interactor(Widget &rootWidget): m_rootWidget{rootWidget} {}
    ~Interactor() override = default;
    Interactor(const Interactor &) = delete;
    Interactor(Interactor &&) = delete;
    auto operator=(const Interactor &) -> Interactor & = delete;
    auto operator=(Interactor &&) -> Interactor & = delete;

protected:
    void onMouseButtonPress(MouseButton button, int x, int y) override;
    void onMouseButtonRelease(MouseButton button, int x, int y) override;
    void onMouseMove(int x, int y) override;
    void onKeyPress(unsigned short key) override;
    void onKeyRelease(unsigned short key) override;
    void onScroll(int dx, int dy) override;
    void onCustomEvent(unsigned int /*id*/, void * /*data*/) override;
    void onDragAndDrop(const std::vector<std::string> &droppedItems) override;
};

} // namespace iloj::gui
