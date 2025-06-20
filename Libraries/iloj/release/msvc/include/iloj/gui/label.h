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
#include <iloj/gpu/uniform.h>
#include <iloj/gui/text.h>
#include <iloj/media/descriptor.h>
#include <iloj/misc/packet.h>

namespace iloj::gui
{
class Label: public Widget
{
protected:
    int m_cornerRadius = 10;
    int m_borderWidth = 2;
    float m_fontSize = 16.F;
    int m_textAlignment = (Alignment::HCenter | Alignment::VCenter);
    gpu::GLColor m_backgroundColor = gpu::Color::None();
    gpu::GLColor m_borderColor = {0.2F, 0.2F, 0.2F, 1.F};
    gpu::GLColor m_fontColor = gpu::Color::White();
    std::pair<iloj::misc::Packet<media::Descriptor::Video>, gpu::Texture2D> m_backgroundImage;
    bool m_hasBackgroundImage = false;
    bool m_keepAspectRatio = true;
    std::string m_text;

public:
    void setCornerRadius(int v) { m_cornerRadius = v; }
    auto getCornerRadius() const -> int { return m_cornerRadius; }
    void setBorderWidth(int v) { m_borderWidth = v; }
    auto getBorderWidth() const -> int { return m_borderWidth; }
    void setFontSize(float v) { m_fontSize = v; }
    auto getFontSize() const -> float { return m_fontSize; }
    void setTextAlignment(int a) { m_textAlignment = a; }
    auto getTextAlignment() const -> int { return m_textAlignment; }
    void setBackgroundColor(const gpu::GLColor &c) { m_backgroundColor = c; }
    auto getBackgroundColor() const -> const gpu::GLColor & { return m_backgroundColor; }
    void setBorderColor(const gpu::GLColor &c) { m_borderColor = c; }
    auto getBorderColor() const -> const gpu::GLColor & { return m_borderColor; }
    void setFontColor(const gpu::GLColor &c) { m_fontColor = c; }
    auto getFontColor() const -> const gpu::GLColor & { return m_fontColor; }
    auto hasBackgroundImage() const -> bool { return m_hasBackgroundImage; }
    void setBackgroundImage(iloj::misc::Packet<media::Descriptor::Video> pkt);
    void loadBackgroundImage(media::Descriptor::Video desc);
    auto getBackgroundImage() const -> const gpu::Texture2D & { return m_backgroundImage.second; }
    void setKeepAspectRatio(bool b) { m_keepAspectRatio = b; }
    auto getKeepAspectRatio() const -> bool { return m_keepAspectRatio; }
    auto hasText() const -> bool { return !m_text.empty(); }
    void setText(std::string v) { m_text = std::move(v); }
    auto getText() const -> const std::string & { return m_text; }
    void toUniformBufferLayout(iloj::gpu::Uniform::Buffer::Layout &layout) const;

protected:
    void draw() override;
    void onResize(int w_old, int h_old, int w_new, int h_new) override;
    void tryLoadBackgroundImage();
};

} // namespace iloj::gui