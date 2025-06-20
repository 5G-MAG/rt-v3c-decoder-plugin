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

#include "types.h"
#include <iloj/gpu/color.h>
#include <iloj/gpu/texture.h>

namespace iloj::gui
{
namespace text
{
static const unsigned MaxChar = 2048;
static const float FontAspectRatio = (13.F / 24.F);
} // namespace text

void print(const gpu::Target<gpu::Texture2D> &target,
           const gpu::ViewPort &viewPort,
           const std::string &s,
           const gpu::Vec2f &position = {0.F, 1.F},
           float fontSize = 24.,
           const gpu::Vec2f &boxSize = {-1.F, -1.F},
           unsigned alignment = (Alignment::Left | Alignment::Top),
           const gpu::GLColor &fontColor = gpu::Color::White(),
           float spacing = 0.25);

} // namespace iloj::gui
