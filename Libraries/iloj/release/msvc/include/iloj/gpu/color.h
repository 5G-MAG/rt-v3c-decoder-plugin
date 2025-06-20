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
#include <functional>

namespace iloj::gpu
{
using ColorMapping = std::function<GLColor(float, float)>;

class Color
{
public:
    static auto None() -> GLColor { return GLColor({0.F, 0.F, 0.F, 0.F}); }
    static auto Invalid() -> GLColor { return GLColor({-1.F, -1.F, -1.F, -1.F}); }

    static auto Black(float alpha = 1.F) -> GLColor { return GLColor({0.F, 0.F, 0.F, alpha}); }
    static auto Gray(float alpha = 1.F) -> GLColor { return GLColor({0.5F, 0.5F, 0.5F, alpha}); }
    static auto Silver(float alpha = 1.F) -> GLColor { return GLColor({0.75F, 0.75F, 0.75F, alpha}); }
    static auto White(float alpha = 1.F) -> GLColor { return GLColor({1.F, 1.F, 1.F, alpha}); }
    static auto Marron(float alpha = 1.F) -> GLColor { return GLColor({0.5F, 0.F, 0.F, alpha}); }
    static auto Red(float alpha = 1.F) -> GLColor { return GLColor({1.F, 0.F, 0.F, alpha}); }
    static auto Olive(float alpha = 1.F) -> GLColor { return GLColor({0.5F, 0.5F, 0.F, alpha}); }
    static auto Yellow(float alpha = 1.F) -> GLColor { return GLColor({1.F, 1.F, 0.F, alpha}); }
    static auto Green(float alpha = 1.F) -> GLColor { return GLColor({0.F, 0.5F, 0.F, alpha}); }
    static auto Lime(float alpha = 1.F) -> GLColor { return GLColor({0.F, 1.F, 0.F, alpha}); }
    static auto Teal(float alpha = 1.F) -> GLColor { return GLColor({0.F, 0.5F, 0.5F, alpha}); }
    static auto Cyan(float alpha = 1.F) -> GLColor { return GLColor({0.F, 1.F, 1.F, alpha}); }
    static auto Navy(float alpha = 1.F) -> GLColor { return GLColor({0.F, 0.F, 0.5F, alpha}); }
    static auto Blue(float alpha = 1.F) -> GLColor { return GLColor({0.F, 0.F, 1.F, alpha}); }
    static auto Purple(float alpha = 1.F) -> GLColor { return GLColor({0.5F, 0.5F, 0.5F, alpha}); }
    static auto Magenta(float alpha = 1.F) -> GLColor { return GLColor({1.F, 0.F, 1.F, alpha}); }
    static auto Orange(float alpha = 1.F) -> GLColor { return GLColor({1.F, 0.5F, 0.F, alpha}); }

    static auto gray(float v, float alpha = 1.F) -> GLColor;
    static auto jet(float v, float alpha = 1.F) -> GLColor;
};

auto GLColorToRGB8(const GLColor &c) -> RGB8UI;
auto buildColorTable(unsigned sz, const ColorMapping &colorMapping, float alpha = 1.F) -> std::vector<GLColor>;
auto buildColorTable(unsigned sz,
                     const ColorMapping &colorMapping,
                     const std::function<float(const GLColor &, const GLColor &)> &distanceFunction,
                     float alpha = 1.F) -> std::vector<GLColor>;
auto mixupColorTable(const std::vector<GLColor> &tableIn,
                     const std::function<float(const GLColor &, const GLColor &)> &distanceFunction)
    -> std::vector<GLColor>;
} // namespace iloj::gpu
