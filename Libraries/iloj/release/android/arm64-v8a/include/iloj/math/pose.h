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

#include <iloj/math/linalg.h>

namespace iloj::math
{
enum class Direction
{
    None,
    Upward,
    Downward,
    Forward,
    Backward,
    Rightward,
    Leftward
};

using Frame = std::array<Direction, 3>;

template<typename T>
auto getRotationMatrix(const Frame &source, const Frame &target) -> stack::Mat3x3<T>;
template<typename T>
auto getForwardLeftwardUpward(const Frame &frame) -> std::array<stack::Vec3<T>, 3>;
template<typename T>
auto getFrame(const std::array<stack::Vec3<T>, 3> &vForwardLeftwardUpward) -> Frame;

template<typename T>
auto getSkewMatix(const stack::Vec3<T> &v) -> stack::Mat3x3<T>;
template<typename T>
auto getRotationMatrixFromAxisAndAngle(stack::Vec3<T> &axis, T &angle) -> stack::Mat3x3<T>;
template<typename T>
auto getPoseMatrixFromRotationAndTranslation(const stack::Mat3x3<T> &R, const stack::Vec3<T> &t) -> stack::Mat4x4<T>;
template<typename T>
auto getRotationAndTranslationFromPoseMatrix(const stack::Mat4x4<T> &M) -> std::pair<stack::Mat3x3<T>, stack::Vec3<T>>;

template<typename T>
class Quaternion: public stack::Vec4<T>
{
public:
    using stack::Vec4<T>::Vec4; // NOLINT
    using stack::Vec4<T>::operator=;
    Quaternion(T x, T y, T z, T w): stack::Vec4<T>({x, y, z, w}) {}
    [[nodiscard]] auto imag() const -> stack::Vec3<T> { return {this->x(), this->y(), this->z()}; }
    [[nodiscard]] auto real() const -> T { return this->w(); }
    static auto fromAxisAndAngle(stack::Vec3<T> axis, T angle) -> Quaternion;
    [[nodiscard]] auto getAxisAndAngle() const -> std::pair<stack::Vec3<T>, T>;
    static auto fromRotationMatrix(const stack::Mat3x3<T> &m) -> Quaternion;
    [[nodiscard]] auto toRotationMatrix() const -> stack::Mat3x3<T>;
    [[nodiscard]] auto inverse() const -> Quaternion;
    auto operator*(const Quaternion &q) const -> Quaternion;
    auto rotate(const stack::Vec3<T> &v) const -> stack::Vec3<T>;
};

template<typename T>
class Pose
{
private:
    Quaternion<T> m_quaternion;
    stack::Vec3<T> m_translation;

public:
    Pose() = default;
    ~Pose() = default;
    Pose(const Quaternion<T> &quaternion, const stack::Vec3<T> &translation)
        : m_quaternion{quaternion}, m_translation{translation}
    {
    }
    Pose(const Pose &) = default;
    Pose(Pose &&) noexcept = default;
    auto operator=(const Pose &) -> Pose & = default;
    auto operator=(Pose &&) noexcept -> Pose & = default;
    auto operator==(const Pose &other) const -> bool
    {
        return ((m_quaternion == other.m_quaternion) && (m_translation == other.m_translation));
    }
    auto operator!=(const Pose &other) const -> bool { return !(*this == other); }
    [[nodiscard]] auto getQuaternion() const -> const Quaternion<T> & { return m_quaternion; }
    [[nodiscard]] auto getTranslation() const -> const stack::Vec3<T> & { return m_translation; }
    [[nodiscard]] auto rotate(const stack::Vec3<T> &axis, T angle) const -> Pose;
    [[nodiscard]] auto translate(const stack::Vec3<T> &translation) const -> Pose;
    static auto fromMatrix(const stack::Mat4x4<T> &M) -> Pose;
    [[nodiscard]] auto toMatrix() const -> stack::Mat4x4<T>;
    [[nodiscard]] auto inverse() const -> Pose;
    [[nodiscard]] auto getTransformedPoint(const stack::Vec3<T> &in) const -> stack::Vec3<T>;
    [[nodiscard]] auto getTransformedDirection(const stack::Vec3<T> &in) const -> stack::Vec3<T>;
    static auto Identity() -> Pose { return Pose({0, 0, 0, 1}, {0, 0, 0}); }
    auto operator*(const Pose &other) const -> Pose;
};

template<typename T>
class Transform
{
private:
    stack::Mat4x4<T> m_transform;

public:
    explicit Transform(const stack::Mat4x4<T> &transform = stack::Mat4x4<T>::eye()): m_transform{transform} {}
    ~Transform() = default;
    Transform(const Transform &) = default;
    Transform(Transform &&) noexcept = default;
    auto operator=(const Transform &) -> Transform & = default;
    auto operator=(Transform &&) noexcept -> Transform & = default;
    auto toMatrix() const -> const stack::Mat4x4<T> & { return m_transform; }
    auto rotate(const stack::Vec3<T> &axis, T angle) const -> Transform;
    auto translate(const stack::Vec3<T> &translation) const -> Transform;
    auto scale(const stack::Vec3<T> &scaling) const -> Transform;
    auto unscale() const -> Transform;
    auto getTransformedPoint(const stack::Vec3<T> &in) const -> stack::Vec3<T>;
    auto getTransformedDirection(const stack::Vec3<T> &in) const -> stack::Vec3<T>;
    static auto Identity() -> Transform { return {}; }
};
} // namespace iloj::math

#include "pose.hpp"
