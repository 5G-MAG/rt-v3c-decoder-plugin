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

#include <iloj/math/matrix.h>
#include <iloj/math/vector.h>
#include <iloj/misc/memory.h>

namespace iloj::gpu
{
using Vec2b = iloj::math::Vec2b;
using Vec2i = iloj::math::Vec2i;
using Vec2u = iloj::math::Vec2u;
using Vec2f = iloj::math::Vec2f;
using Vec3b = iloj::math::Vec3b;
using Vec3i = iloj::math::Vec3i;
using Vec3u = iloj::math::Vec3u;
using Vec3f = iloj::math::Vec3f;
using Vec4b = iloj::math::Vec4b;
using Vec4i = iloj::math::Vec4i;
using Vec4u = iloj::math::Vec4u;
using Vec4f = iloj::math::Vec4f;
template<typename T>
using Vec = iloj::math::Vec<T>;

using Mat2x2f = iloj::math::Mat2x2f;
using Mat3x3f = iloj::math::Mat3x3f;
using Mat4x4f = iloj::math::Mat4x4f;
template<typename T>
using Mat = iloj::math::Mat<T>;

using GLColor = Vec4f;
using RGB8UI = iloj::math::stack::Vec3<std::uint8_t>;

using LineIndex = std::array<unsigned, 2>;
using TriangleIndex = std::array<unsigned, 3>;
using QuadIndex = std::array<unsigned, 4>;

struct FlipMode
{
    enum
    {
        None = 0,
        Horizontal = 1,
        Vertical = 2,
        All = 3
    };
    static auto fromBoolean(bool horizontal, bool vertical) -> unsigned
    {
        return ((horizontal ? unsigned{Horizontal} : 0U) | (vertical ? unsigned{Vertical} : 0U));
    }
    static auto isHorizontal(unsigned flip) -> int { return ((flip & unsigned{Horizontal}) != 0) ? 1 : 0; }
    static auto isVertical(unsigned flip) -> int { return ((flip & unsigned{Vertical}) != 0) ? 1 : 0; }
};

template<typename T>
using Target = iloj::misc::Reference<T>;
template<typename T>
using TargetList = std::vector<Target<T>>;

} // namespace iloj::gpu
