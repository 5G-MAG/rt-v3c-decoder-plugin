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

#include "functions.h"

namespace iloj::gpu
{
class ViewPort
{
private:
    unsigned m_left = 0, m_bottom = 0;
    unsigned m_width = 0, m_height = 0;

public:
    ViewPort() = default;
    ~ViewPort() = default;
    ViewPort(unsigned w, unsigned h, unsigned l = 0, unsigned b = 0): m_left(l), m_bottom(b), m_width(w), m_height(h) {}
    ViewPort(const ViewPort &) = default;
    ViewPort(ViewPort &&) = default;
    auto operator=(const ViewPort &) -> ViewPort & = default;
    auto operator=(ViewPort &&) -> ViewPort & = default;
    [[nodiscard]] auto getWidth() const -> unsigned { return m_width; }
    [[nodiscard]] auto getHeight() const -> unsigned { return m_height; }
    [[nodiscard]] auto getLeft() const -> unsigned { return m_left; }
    [[nodiscard]] auto getBottom() const -> unsigned { return m_bottom; }
    void resize(unsigned w, unsigned h, unsigned l = 0, int b = 0)
    {
        m_left = l;
        m_bottom = b;
        m_width = w;
        m_height = h;
    }
    [[nodiscard]] auto getAspectRatio() const -> float { return (float) m_width / (float) m_height; }
    void apply() const { glViewport(m_left, m_bottom, m_width, m_height); }
    [[nodiscard]] auto rescale(float aspectRatio) const -> ViewPort;
    [[nodiscard]] auto getHalfLeft() const -> ViewPort;
    [[nodiscard]] auto getHalfRight() const -> ViewPort;
    [[nodiscard]] auto getHalfTop() const -> ViewPort;
    [[nodiscard]] auto getHalfBottom() const -> ViewPort;
};
} // namespace iloj::gpu
