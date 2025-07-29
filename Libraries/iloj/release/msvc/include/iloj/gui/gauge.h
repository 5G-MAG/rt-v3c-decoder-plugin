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
#include <iloj/gpu/color.h>
#include <iloj/gpu/uniform.h>

namespace iloj::gui
{
class Gauge: public Widget
{
public:
    struct ColorMode
    {
        enum
        {
            Mono = 0,
            Jet = 1
        };
    };
    struct FillMode
    {
        enum
        {
            Off = 0,
            On = 1
        };
    };

protected:
    int m_borderWidth = 2;
    int m_gapSize = 5;
    int m_stepSize = 15;
    float m_value = 0.5F;
    gpu::GLColor m_backgroundColor = gpu::Color::None();
    gpu::GLColor m_borderColor = {0.2F, 0.2F, 0.2F, 1.F};
    gpu::GLColor m_foregroundColor = gpu::Color::Blue();
    int m_colorMode = ColorMode::Mono;
    int m_fillMode = FillMode::On;

public:
    void setBorderWidth(int v) { m_borderWidth = v; }
    [[nodiscard]] auto getBorderWidth() const -> int { return m_borderWidth; }
    void setGapSize(int v) { m_gapSize = v; }
    [[nodiscard]] auto getGapSize() const -> int { return m_gapSize; }
    void setStepSize(int v) { m_stepSize = v; }
    [[nodiscard]] auto getStepSize() const -> int { return m_stepSize; }
    void setValue(float v) { m_value = v; }
    [[nodiscard]] auto getValue() const -> float { return m_value; }
    void setBackgroundColor(gpu::GLColor c) { m_backgroundColor = c; }
    [[nodiscard]] auto getBackgroundColor() const -> const gpu::GLColor & { return m_backgroundColor; }
    void setBorderColor(gpu::GLColor c) { m_borderColor = c; }
    [[nodiscard]] auto getBorderColor() const -> const gpu::GLColor & { return m_borderColor; }
    void setForegroundColor(gpu::GLColor c) { m_foregroundColor = c; }
    [[nodiscard]] auto getForegrounColor() const -> const gpu::GLColor & { return m_foregroundColor; }
    void setColorMode(int v) { m_colorMode = v; }
    [[nodiscard]] auto getColorMode() const -> int { return m_colorMode; }
    void setFillMode(int v) { m_fillMode = v; }
    [[nodiscard]] auto getFillMode() const -> int { return m_fillMode; }
    void toUniformBufferLayout(iloj::gpu::Uniform::Buffer::Layout &layout) const;

protected:
    void draw() override;
};

} // namespace iloj::gui
