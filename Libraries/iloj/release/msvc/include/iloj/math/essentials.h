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

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <limits>
#include <vector>

//! \brief pi related constant
#ifndef M_PI
constexpr double M_PI = 3.14159265358979323846;
#endif
#ifndef M_PI_2
constexpr double M_PI_2 = 1.57079632679;
#endif
constexpr double M_2PI = M_PI * 2.0;
constexpr double M_4PI = M_PI * 4.0;

constexpr float K_PI = static_cast<float>(M_PI);
constexpr float K_PI_2 = static_cast<float>(M_PI_2);
constexpr float K_2PI = static_cast<float>(M_2PI);
constexpr float K_4PI = static_cast<float>(M_4PI);

namespace iloj::math
{
//! \brief Degrees to radian conversion
template<typename T>
auto deg2rad(T x) -> T
{
    return x * static_cast<T>(M_PI / 180.);
}

//! \brief Radian to degree conversion
template<typename T>
auto rad2deg(T x) -> T
{
    return x * static_cast<T>(180. / M_PI);
}

//! \brief Returns squared value
template<typename T>
auto sqr(T val) -> T
{
    return val * val;
}

//! \brief Returns cubeb value
template<typename T>
auto cube(T val) -> T
{
    return val * val * val;
}

//! \brief Returns parameter sign
template<typename T>
auto sgn(T val) -> int
{
    return int(T(0) < val) - int(val < T(0));
}

//! \brief Returns val clamped in [min, max]
template<typename T>
auto clamp(T val, T min, T max) -> T
{
    if (val < min)
    {
        return min;
    }
    if (max < val)
    {
        return max;
    }

    return val;
}

//! \brief Checks if val is in range [min, max]
template<typename T>
auto inRange(T val, T min, T max) -> bool
{
    return ((min <= val) && (val <= max));
}

//! \brief Checks true if val is null with respect to given tolerance
template<typename T>
auto is_zero(T val, T tol = std::numeric_limits<T>::epsilon()) -> bool
{
    return (std::abs(val) < tol);
}

//! \brief Returns input angle squashed in [-pi, pi]
template<typename T>
inline auto squash(T a) -> T
{
    while (static_cast<T>(M_PI) < a)
    {
        a -= static_cast<T>(M_2PI);
    }
    while (a < -static_cast<T>(M_PI))
    {
        a += static_cast<T>(M_2PI);
    }
    return a;
}

//! \brief Returns integral
inline auto ipow(int base, unsigned int exp) -> int
{
    int result = 1;
    for (;;)
    {
        if ((exp & 1U) != 0U)
        {
            result *= base;
        }
        exp >>= 1U;
        if (exp == 0U)
        {
            break;
        }
        base *= base;
    }
    return result;
}

//! \brief Returns abs of a
template<typename T>
auto abs(T a) -> T
{
    if constexpr (std::is_unsigned<T>::value)
    {
        return a;
    }
    else
    {
        return (a < 0) ? -a : a;
    }
}

//! \brief Returns greatest common diviser of a and b
template<typename T>
auto gcd(T a, T b) -> T
{
    return (b == 0) ? a : gcd(b, a % b);
}

//! \brief Returns least common multiple of a and b
template<typename T>
auto lcm(T a, T b) -> T
{
    return abs(a * b) / gcd(a, b);
}

//! \brief Conversion from pixel per degree to pixel per steradian
template<typename T>
auto ppd2pps(T ppd) -> T
{
    return sqr(static_cast<T>(180. / M_PI) * ppd);
}

//! \brief Conversion from pixel per steradian to pixel per degree
template<typename T>
auto pps2ppd(T pps) -> T
{
    return std::sqrt(pps) * static_cast<T>(M_PI / 180.);
}

//! \brief Returns the solid angle of a cone with its apex at the apex of the solid angle, and with apex angle theta
template<typename T>
auto solid(T theta) -> T
{
    return static_cast<T>(M_2PI) * (static_cast<T>(1) - std::cos(theta / static_cast<T>(2)));
}

//! \brief Returns the solid angle of a latitude-longitude rectangle on a globe
template<typename T>
auto solid(T thetaWest, T thetaEast, T phiSouth, T phiNorth) -> T
{
    return (std::sin(phiNorth) - std::sin(phiSouth)) * (thetaEast - thetaWest);
}

//! \brief Returns conjugate value
template<typename T,
         typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value, int>::type = 0>
auto conjugate(T v) -> T
{
    return v;
}
template<typename T,
         typename std::enable_if<!(std::is_integral<T>::value || std::is_floating_point<T>::value), int>::type = 0>
auto conjugate(T v) -> T
{
    return std::conj(v);
}

//! \brief Return the first multiple of alignment greater or equal than value
template<typename T>
auto align(T value, T alignment) -> T
{
    T misalignment = value % alignment;
    return (misalignment != 0) ? (value + (alignment - misalignment)) : value;
}

//! \brief Returns the next power of two greater than or equal to n.
inline auto nextPow2(unsigned n) -> unsigned
{
    if (0 < n)
    {
        n -= 1U;
        n |= (n >> 1U);
        n |= (n >> 2U);
        n |= (n >> 4U);
        n |= (n >> 8U);
        n |= (n >> 16U);
        n++;

        return n;
    }

    return 1U;
}

//! \brief Returns the number of bits required to encode range
inline auto ceilLog2(long unsigned range) -> unsigned
{
    if (range == 0)
    {
        return 0;
    }

    --range;

    unsigned bits = 0;

    while (range > 0)
    {
        ++bits;
        range /= 2;
    }

    return bits;
}

//! \brief Returns the histogram of the input data on the specified number of bins with optional explicit bounds
template<typename Iterator1, typename Iterator2>
void histogram(Iterator1 first,
               Iterator1 last,
               Iterator2 result,
               std::size_t nbBins,
               const std::array<typename std::iterator_traits<Iterator1>::value_type, 2> &bounds = {})
{
    using T1 = typename std::iterator_traits<Iterator1>::value_type;

    auto hasExplicitBounds = (bounds[0] < bounds[1]);
    std::vector<T1> data(first, last);

    std::sort(data.begin(), data.end());

    auto minValue = hasExplicitBounds ? bounds[0] : data.front();
    auto maxValue = hasExplicitBounds ? bounds[1] : data.back();

    auto binWidth = (maxValue - minValue) / static_cast<T1>(nbBins);
    auto v = binWidth;

    auto iter1 = std::lower_bound(data.begin(), data.end(), minValue);
    auto iter2 = result;
    auto iter3 = result + nbBins;

    while ((v <= *iter1) && (iter2 != iter3))
    {
        *iter2++ = 0;

        if (iter2 < (iter3 - 1))
        {
            v += binWidth;
        }
        else
        {
            v = maxValue;
        }
    }

    while (iter2 != iter3)
    {
        auto back = (iter2 == (iter3 - 1));

        auto next = back ? std::upper_bound(iter1, data.end(), v) : std::lower_bound(iter1, data.end(), v);
        *iter2 = std::distance(iter1, next);

        v = back ? maxValue : (v + binWidth);

        iter1 = next;
        iter2++;
    }
}

template<typename Iterator>
auto histogram(Iterator first,
               Iterator last,
               std::size_t nbBins,
               const std::array<typename std::iterator_traits<Iterator>::value_type, 2> &bounds = {})
    -> std::vector<std::size_t>
{
    std::vector<std::size_t> h(nbBins);
    histogram(first, last, h.begin(), nbBins, bounds);
    return h;
}

} // namespace iloj::math
