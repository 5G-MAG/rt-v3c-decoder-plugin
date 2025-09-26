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

#include "widget.h"
#include <iloj/gpu/delegate.h>

namespace iloj::gui
{
template<typename W>
class Delegate: public gpu::Delegate
{
private:
    W m_rootWidget;

public:
    template<typename... Args>
    Delegate(Args &&... args): m_rootWidget{std::forward<Args>(args)...}
    {
        m_rootWidget.m_root = &m_rootWidget;
    }
    ~Delegate() override = default;
    Delegate(const Delegate &) = default;
    Delegate(Delegate &&) noexcept = default;
    auto operator=(const Delegate &) -> Delegate & = default;
    auto operator=(Delegate &&) noexcept -> Delegate & = default;
    auto getRootWidget() -> W & { return m_rootWidget; }

protected:
    void update() override { m_rootWidget.drawTree(); }
    void reshape(int w, int h) override { m_rootWidget.onResizeTree(0, 0, w, h); }
};

} // namespace iloj::gui
