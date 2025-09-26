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
#include <iloj/gpu/texture.h>
#include <iloj/media/descriptor.h>
#include <iloj/misc/packet.h>

namespace iloj::gui
{
class Background: public Widget
{
protected:
    gpu::GLColor m_backgroundColor = gpu::Color::Invalid();
    std::pair<iloj::misc::Packet<media::Descriptor::Video>, gpu::Texture2D> m_backgroundImage;
    bool m_hasBackgroundImage = false;

public:
    void setBackgroundColor(const gpu::GLColor &c) { m_backgroundColor = c; }
    auto getBackgroundColor() const -> const gpu::GLColor & { return m_backgroundColor; }
    void setBackgroundImage(iloj::misc::Packet<media::Descriptor::Video> pkt)
    {
        m_hasBackgroundImage = (pkt && pkt->isAllocated());
        m_backgroundImage.first = std::move(pkt);
    }

protected:
    void draw() override;
    void onResize(int /*w_old*/, int /*h_old*/, int w_new, int h_new) override { resize(w_new, h_new); }
};

} // namespace iloj::gui