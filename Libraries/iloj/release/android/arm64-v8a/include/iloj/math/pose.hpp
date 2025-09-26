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

namespace iloj::math
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
auto getInternalVectorFromDirection(Direction d) -> stack::Vec3<T>
{
    stack::Vec3<T> v;

    switch (d)
    {
        case Direction::Upward:
        {
            v = {0, 1, 0};
            break;
        }
        case Direction::Downward:
        {
            v = {0, -1, 0};
            break;
        }
        case Direction::Forward:
        {
            v = {0, 0, 1};
            break;
        }
        case Direction::Backward:
        {
            v = {0, 0, -1};
            break;
        }
        case Direction::Rightward:
        {
            v = {-1, 0, 0};
            break;
        }
        case Direction::Leftward:
        {
            v = {1, 0, 0};
            break;
        }
        default:;
    }

    return v;
}

template<typename T>
auto getNewToInternalRotationMatrix(const Frame &frame) -> stack::Mat3x3<T>
{
    stack::Mat3x3<T> out = stack::Mat3x3<T>::eye();

    for (unsigned i = 0; i < 3; i++)
    {
        auto v = getInternalVectorFromDirection<T>(frame[i]);
        std::copy(v.begin(), v.end(), out.col_begin(i));
    }

    return out;
}

template<typename T>
auto getRotationMatrix(const Frame &source, const Frame &target) -> stack::Mat3x3<T>
{
    auto source2intern = getNewToInternalRotationMatrix<T>(source);
    auto target2intern = getNewToInternalRotationMatrix<T>(target);
    return matprod(target2intern, 'T', source2intern, 'N');
}

template<typename T>
auto getForwardLeftwardUpward(const Frame &frame) -> std::array<stack::Vec3<T>, 3>
{
    std::array<stack::Vec3<T>, 3> out;

    for (unsigned i = 0; i < 3; i++)
    {
        stack::Vec3<T> axis{};

        axis[i] = 1;

        switch (frame[i])
        {
            case Direction::Forward:
            {
                out[0] = axis;
                break;
            }
            case Direction::Backward:
            {
                out[0] = -axis;
                break;
            }
            case Direction::Leftward:
            {
                out[1] = axis;
                break;
            }
            case Direction::Rightward:
            {
                out[1] = -axis;
                break;
            }
            case Direction::Upward:
            {
                out[2] = axis;
                break;
            }
            case Direction::Downward:
            {
                out[2] = -axis;
                break;
            }
            default:;
        }
    }

    return out;
}

template<typename T>
auto getFrame(const std::array<stack::Vec3<T>, 3> &vForwardLeftwardUpward) -> Frame
{
    static const std::array<std::pair<Direction, Direction>, 3> directions = {
        std::make_pair(Direction::Forward, Direction::Backward),
        std::make_pair(Direction::Leftward, Direction::Rightward),
        std::make_pair(Direction::Upward, Direction::Downward)};

    Frame out;

    for (unsigned i = 0; i < 3; i++)
    {
        const auto &v = vForwardLeftwardUpward[i];
        auto iter = std::find_if(v.begin(), v.end(), [](T x) { return !is_zero(x); }); // NOLINT
        auto j = std::distance(v.begin(), iter);

        out[j] = (0. < *iter) ? directions[i].first : directions[i].second;
    }

    return out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
auto getSkewMatix(const stack::Vec3<T> &v) -> stack::Mat3x3<T>
{
    return stack::Mat3x3<T>{0, -v[2], v[1], v[2], 0, -v[0], -v[1], v[0], 0};
}

template<typename T>
auto getRotationMatrixFromAxisAndAngle(stack::Vec3<T> &axis, T &angle) -> stack::Mat3x3<T>
{
    stack::Mat3x3<T> m;
    auto c = std::cos(angle);
    auto s = std::sin(angle);
    T t1;
    T t2;

    m(0, 0) = (axis[0] * axis[0]) * (1 - c) + c;
    m(1, 1) = (axis[1] * axis[1]) * (1 - c) + c;
    m(2, 2) = (axis[2] * axis[2]) * (1 - c) + c;

    t1 = axis[0] * axis[1] * (1 - c);
    t2 = axis[2] * s;
    m(0, 1) = t1 - t2;
    m(1, 0) = t1 + t2;

    t1 = axis[0] * axis[2] * (1 - c);
    t2 = axis[1] * s;
    m(0, 2) = t1 + t2;
    m(2, 0) = t1 - t2;

    t1 = axis[1] * axis[2] * (1 - c);
    t2 = axis[0] * s;
    m(1, 2) = t1 - t2;
    m(2, 1) = t1 + t2;

    return m;
}

template<typename T>
auto getPoseMatrixFromRotationAndTranslation(const stack::Mat3x3<T> &R, const stack::Vec3<T> &t) -> stack::Mat4x4<T>
{
    stack::Mat4x4<T> M;

    M(0, 0) = R(0, 0);
    M(0, 1) = R(0, 1);
    M(0, 2) = R(0, 2);
    M(0, 3) = t[0];
    M(1, 0) = R(1, 0);
    M(1, 1) = R(1, 1);
    M(1, 2) = R(1, 2);
    M(1, 3) = t[1];
    M(2, 0) = R(2, 0);
    M(2, 1) = R(2, 1);
    M(2, 2) = R(2, 2);
    M(2, 3) = t[2];
    M(3, 0) = 0;
    M(3, 1) = 0;
    M(3, 2) = 0;
    M(3, 3) = 1;

    return M;
}

template<typename T>
auto getRotationAndTranslationFromPoseMatrix(const stack::Mat4x4<T> &M) -> std::pair<stack::Mat3x3<T>, stack::Vec3<T>>
{
    std::pair<stack::Mat3x3<T>, stack::Vec3<T>> out;
    auto &R = out.first;
    auto &t = out.second;

    R(0, 0) = M(0, 0);
    R(0, 1) = M(0, 1);
    R(0, 2) = M(0, 2);
    t[0] = M(0, 3);
    R(1, 0) = M(1, 0);
    R(1, 1) = M(1, 1);
    R(1, 2) = M(1, 2);
    t[1] = M(1, 3);
    R(2, 0) = M(2, 0);
    R(2, 1) = M(2, 1);
    R(2, 2) = M(2, 2);
    t[2] = M(2, 3);

    return out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
auto Quaternion<T>::fromAxisAndAngle(stack::Vec3<T> axis, T angle) -> Quaternion
{
    auto ha = angle / 2;
    auto sinha = std::sin(ha);
    auto cosha = std::cos(ha);
    return {axis[0] * sinha, axis[1] * sinha, axis[2] * sinha, cosha};
}

template<typename T>
auto Quaternion<T>::getAxisAndAngle() const -> std::pair<stack::Vec3<T>, T>
{
    std::pair<stack::Vec3<T>, T> out;
    auto &axis = out.first;
    auto &angle = out.second;
    auto sinha = std::sqrt(this->x() * this->x() + this->y() * this->y() + this->z() * this->z());
    auto cosha = this->w();

    if (sinha < std::numeric_limits<T>::epsilon())
    {
        angle = 0;
        axis[0] = 0;
        axis[1] = 0;
        axis[2] = 1;
    }
    else
    {
        angle = 2 * std::atan2(sinha, cosha);
        axis[0] = this->x() / sinha;
        axis[1] = this->y() / sinha;
        axis[2] = this->z() / sinha;
    }

    return out;
}

template<typename T>
auto Quaternion<T>::fromRotationMatrix(const stack::Mat3x3<T> &m) -> Quaternion
{
    Quaternion q;
    auto U = m(0, 0) + m(1, 1) + m(2, 2);

    if (U > 0)
    {
        auto S = std::sqrt(U + 1) * 2;

        q.w() = S / 4;
        q.x() = (m(2, 1) - m(1, 2)) / S;
        q.y() = (m(0, 2) - m(2, 0)) / S;
        q.z() = (m(1, 0) - m(0, 1)) / S;
    }
    else if ((m(0, 0) > m(1, 1)) && (m(0, 0) > m(2, 2)))
    {
        auto S = std::sqrt(1 + m(0, 0) - m(1, 1) - m(2, 2)) * 2;

        q.w() = (m(2, 1) - m(1, 2)) / S;
        q.x() = S / 4;
        q.y() = (m(0, 1) + m(1, 0)) / S;
        q.z() = (m(0, 2) + m(2, 0)) / S;
    }
    else if (m(1, 1) > m(2, 2))
    {
        auto S = std::sqrt(1 + m(1, 1) - m(0, 0) - m(2, 2)) * 2;

        q.w() = (m(0, 2) - m(2, 0)) / S;
        q.x() = (m(0, 1) + m(1, 0)) / S;
        q.y() = S / 4;
        q.z() = (m(1, 2) + m(2, 1)) / S;
    }
    else
    {
        auto S = std::sqrt(1.0 + m(2, 2) - m(0, 0) - m(1, 1)) * 2;

        q.w() = (m(1, 0) - m(0, 1)) / S;
        q.x() = (m(0, 2) + m(2, 0)) / S;
        q.y() = (m(1, 2) + m(2, 1)) / S;
        q.z() = S / 4;
    }

    return q;
}

template<typename T>
auto Quaternion<T>::toRotationMatrix() const -> stack::Mat3x3<T>
{
    auto p = getAxisAndAngle();
    return getRotationMatrixFromAxisAndAngle(p.first, p.second);
}

template<typename T>
auto Quaternion<T>::inverse() const -> Quaternion
{
    auto fNorm = norm2(*this);

    if (fNorm > 0)
    {
        auto fInvNorm = 1 / fNorm;
        return {-this->x() * fInvNorm, -this->y() * fInvNorm, -this->z() * fInvNorm, this->w() * fInvNorm};
    }

    return {};
}

template<typename T>
auto Quaternion<T>::operator*(const Quaternion &q) const -> Quaternion
{
    auto qx = this->w() * q[0] + this->x() * q[3] + this->y() * q[2] - this->z() * q[1];
    auto qy = this->w() * q[1] - this->x() * q[2] + this->y() * q[3] + this->z() * q[0];
    auto qz = this->w() * q[2] + this->x() * q[1] - this->y() * q[0] + this->z() * q[3];
    auto qw = this->w() * q[3] - this->x() * q[0] - this->y() * q[1] - this->z() * q[2];

    return {qx, qy, qz, qw};
}

template<typename T>
auto Quaternion<T>::rotate(const stack::Vec3<T> &v) const -> stack::Vec3<T>
{
    auto u = imag();
    auto s = real();

    return 2 * dot(u, v) * u + (s * s - dot(u, u)) * v + 2 * s * stack::cross(u, v);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
auto Pose<T>::rotate(const stack::Vec3<T> &axis, T angle) const -> Pose
{
    auto q = Quaternion<T>::fromAxisAndAngle(axis, angle);

    return {q * this->m_quaternion, q.rotate(this->m_translation)};
}

template<typename T>
auto Pose<T>::translate(const stack::Vec3<T> &translation) const -> Pose
{
    return {this->m_quaternion, this->m_translation + translation};
}

template<typename T>
auto Pose<T>::fromMatrix(const stack::Mat4x4<T> &M) -> Pose
{
    auto p = getRotationAndTranslationFromPoseMatrix(M);

    return {Quaternion<T>::fromRotationMatrix(p.first), p.second};
}

template<typename T>
auto Pose<T>::toMatrix() const -> stack::Mat4x4<T>
{
    return getPoseMatrixFromRotationAndTranslation(this->m_quaternion.toRotationMatrix(), m_translation);
}

template<typename T>
auto Pose<T>::inverse() const -> Pose
{
    auto q = this->m_quaternion.inverse();

    return {q, q.rotate(-this->m_translation)};
}

template<typename T>
auto Pose<T>::getTransformedPoint(const stack::Vec3<T> &in) const -> stack::Vec3<T>
{
    return (this->m_quaternion.rotate(in) + this->m_translation);
}

template<typename T>
auto Pose<T>::getTransformedDirection(const stack::Vec3<T> &in) const -> stack::Vec3<T>
{
    return this->m_quaternion.rotate(in);
}

template<typename T>
auto Pose<T>::operator*(const Pose &other) const -> Pose
{
    return {this->m_quaternion * other.m_quaternion,
            this->m_quaternion.rotate(other.m_translation) + this->m_translation};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
auto Transform<T>::rotate(const stack::Vec3<T> &axis, T angle) const -> Transform
{
    return Transform{getPoseMatrixFromRotationAndTranslation(getRotationMatrixFromAxisAndAngle(axis, angle), {}) *
                     this->m_transform};
}

template<typename T>
auto Transform<T>::translate(const stack::Vec3<T> &translation) const -> Transform
{
    stack::Mat4x4<T> transformed = this->m_transform;

    std::transform(
        translation.begin(), translation.end(), transformed.col_begin(3), transformed.col_begin(3), [](T t, T v) {
            return (t + v);
        });

    return Transform{transformed};
}

template<typename T>
auto Transform<T>::scale(const stack::Vec3<T> &scaling) const -> Transform
{
    return Transform{this->m_transform * stack::Mat4x4<T>::diag({scaling.x(), scaling.y(), scaling.z(), T(1)})};
}

template<typename T>
auto Transform<T>::unscale() const -> Transform
{
    stack::Mat4x4<T> transformed = this->m_transform;

    for (unsigned i = 0; i < 3; i++)
    {
        T s = std::sqrt(dot_product(transformed.col_begin(i), transformed.col_begin(i) + 3, transformed.col_begin(i)));
        std::transform(transformed.col_begin(i), transformed.col_begin(i) + 3, transformed.col_begin(i), [s](float v) {
            return (v / s);
        });
    }

    return Transform{transformed};
}

template<typename T>
auto Transform<T>::getTransformedPoint(const stack::Vec3<T> &in) const -> stack::Vec3<T>
{
    stack::Vec3<T> out;

    for (unsigned i = 0; i < 3; i++)
    {
        out[i] = dot_product(in.begin(), in.end(), this->m_transform.row_begin(i)) + this->m_transform(i, 3);
    }

    return out;
}

template<typename T>
auto Transform<T>::getTransformedDirection(const stack::Vec3<T> &in) const -> stack::Vec3<T>
{
    stack::Vec3<T> out;

    for (unsigned i = 0; i < 3; i++)
    {
        out[i] = dot_product(in.begin(), in.end(), this->m_transform.row_begin(i));
    }

    return out;
}
} // namespace iloj::math
