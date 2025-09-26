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

#include "label.h"

namespace iloj::gui
{
class Clickable: public Label
{
public:
    struct StateId
    {
        enum
        {
            None = 0,
            Hovered = 1,
            Pressed = 2
        };
    };

protected:
    gpu::GLColor m_hoverColor = {1., 1., 1., 1.};
    gpu::GLColor m_pressColor = {1., 1., 1., 1.};

protected:
    unsigned m_state = StateId::None;

public:
    void setHoverColor(const gpu::GLColor &c) { m_hoverColor = c; }
    auto getHoverColor() const -> const gpu::GLColor & { return m_hoverColor; }
    void setPressColor(const gpu::GLColor &c) { m_pressColor = c; }
    auto getPressColor() const -> const gpu::GLColor & { return m_pressColor; }
    void toUniformBufferLayout(iloj::gpu::Uniform::Buffer::Layout &layout) const;

protected:
    void draw() override;
    auto onMouseButtonPress(gpu::Interactor::MouseButton button, int x, int y) -> bool override;
    void onMouseButtonRelease(gpu::Interactor::MouseButton button, int x, int y) override;
    void onMouseEntering(int x, int y) override;
    void onMouseLeaving(int x, int y) override;
};

} // namespace iloj::gui
