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
#include <iloj/gui/text.h>

namespace iloj::gui
{
class Input: public Widget
{
protected:
    int m_cornerRadius = 0;
    int m_borderWidth = 2;
    float m_fontSize = 16.F;
    int m_textAlignment = (Alignment::HCenter | Alignment::VCenter);
    gpu::GLColor m_backgroundColor = gpu::Color::White();
    gpu::GLColor m_borderColor = gpu::Color::None();
    gpu::GLColor m_focusColor = gpu::Color::Yellow();
    gpu::GLColor m_fontColor = gpu::Color::Black();
    std::string m_text;
    std::function<void()> m_onReturnKeyCallback;

public:
    void setCornerRadius(int v) { m_cornerRadius = v; }
    [[nodiscard]] auto getCornerRadius() const -> int { return m_cornerRadius; }
    void setBorderWidth(int v) { m_borderWidth = v; }
    [[nodiscard]] auto getBorderWidth() const -> int { return m_borderWidth; }
    void setFontSize(float v) { m_fontSize = v; }
    [[nodiscard]] auto getFontSize() const -> float { return m_fontSize; }
    void setTextAlignment(int a) { m_textAlignment = a; }
    [[nodiscard]] auto getTextAlignment() const -> int { return m_textAlignment; }
    void setBackgroundColor(const gpu::GLColor &c) { m_backgroundColor = c; }
    [[nodiscard]] auto getBackgroundColor() const -> const gpu::GLColor & { return m_backgroundColor; }
    void setBorderColor(const gpu::GLColor &c) { m_borderColor = c; }
    [[nodiscard]] auto getBorderColor() const -> const gpu::GLColor & { return m_borderColor; }
    void setFontColor(const gpu::GLColor &c) { m_fontColor = c; }
    [[nodiscard]] auto getFontColor() const -> const gpu::GLColor & { return m_fontColor; }
    [[nodiscard]] auto hasText() const -> bool { return !m_text.empty(); }
    void setText(std::string v) { m_text = std::move(v); }
    [[nodiscard]] auto getText() const -> const std::string & { return m_text; }
    void setOnReturnKeyCallback(const std::function<void()> &f) { m_onReturnKeyCallback = f; }
    void toUniformBufferLayout(iloj::gpu::Uniform::Buffer::Layout &layout) const;

protected:
    void draw() override;
    auto onMouseButtonPress(gpu::Interactor::MouseButton button, int x, int y) -> bool override;
    auto onKeyPress(unsigned short key) -> bool override;
};

} // namespace iloj::gui