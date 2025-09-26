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

#include "clickable.h"

namespace iloj::gui
{
class Selectable: public Clickable
{
public:
    struct StateId: public Clickable::StateId
    {
        enum
        {
            Selected = 4
        };
    };

protected:
    gpu::GLColor m_selectionColor = {1., 0., 0., 1.};
    std::function<void()> m_onSelectionEnabledCallback;
    std::function<void()> m_onSelectionDisabledCallback;

public:
    void setSelectionColor(const gpu::GLColor &c) { m_selectionColor = c; }
    auto getSelectionColor() const -> const gpu::GLColor & { return m_selectionColor; }
    auto isSelected() const -> bool { return ((m_state & StateId::Selected) != 0); }
    void select(bool lazy = false);
    void discard(bool lazy = false);
    void setOnSelectionEnabledCallback(const std::function<void()> &f) { m_onSelectionEnabledCallback = f; }
    void setOnSelectionDisabledCallback(const std::function<void()> &f) { m_onSelectionDisabledCallback = f; }
    void toUniformBufferLayout(iloj::gpu::Uniform::Buffer::Layout &layout) const;

protected:
    void draw() override;
    auto onMouseButtonPress(gpu::Interactor::MouseButton button, int x, int y) -> bool override;
    void onMouseButtonRelease(gpu::Interactor::MouseButton button, int x, int y) override;
};

} // namespace iloj::gui
