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
class Frame: public Label
{
protected:
    int m_titleOffset = 10;
    int m_titleMargin = 10;
    int m_titleLeft = 0;
    int m_titleRight = 0;

public:
    void setTitleOffset(int v) { m_titleOffset = v; }
    auto getTitleOffset() const -> int { return m_titleOffset; }
    void setTitleMargin(int v) { m_titleMargin = v; }
    auto getTitleMargin() const -> int { return m_titleMargin; }
    void toUniformBufferLayout(iloj::gpu::Uniform::Buffer::Layout &layout) const;

protected:
    void draw() override;
};

} // namespace iloj::gui
