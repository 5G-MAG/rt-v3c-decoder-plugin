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

#include <iloj/math/essentials.h>
#include <iloj/misc/array.h>

namespace iloj::math
{
namespace Vector
{
template<typename A>
class Interface: public A
{
public:
    using size_type = iloj::misc::Array::size_type;
    using const_row_iterator = typename A::const_dim_iterator;
    using row_iterator = typename A::dim_iterator;
    using const_column_iterator = typename A::const_dim_iterator;
    using column_iterator = typename A::dim_iterator;
    template<typename U>
    using promoted_type = Interface<typename A::template promoted_type<U>>;

public:
    using A::A; // NOLINT
    using A::operator=;
    Interface(const A &a): A(a) {}
    Interface(A &&a): A(std::move(a)) {}
    template<typename ITER>
    Interface(ITER first, ITER last)
    {
        resize(std::distance(first, last));
        std::copy(first, last, A::begin());
    }
    auto operator=(const A &a) -> Interface &
    {
        A::operator=(a);
        return *this;
    }
    auto operator=(A &&a) -> Interface &
    {
        A::operator=(std::move(a));
        return *this;
    }
    //! \brief Returns the number of rows of the matrix.
    [[nodiscard]] auto m() const -> size_type { return A::size(0); }
    //! \brief Returns the number of columns of the matrix.
    [[nodiscard]] auto n() const -> size_type { return 1; }
    //! \brief Overloaded resize operator.
    using A::resize;
    void resize(size_type a, size_type /*unused*/ = 1) { A::resize({a}); }
    //! \brief Returns an iterator to the first element of the ith row.
    [[nodiscard]] auto row_begin(size_type i) const -> const_row_iterator { return const_row_iterator(A::begin() + i); }
    auto row_begin(size_type i) -> row_iterator { return row_iterator(A::begin() + i); }
    //! \brief Returns a const iterator to the first element of the ith row.
    [[nodiscard]] auto crow_begin(size_type i) const -> const_row_iterator
    {
        return const_row_iterator(A::begin() + i);
    }
    //! \brief Returns an iterator to the first element after the end of the ith
    //! row.
    [[nodiscard]] auto row_end(size_type i) const -> const_row_iterator
    {
        return const_row_iterator(A::begin() + (i + 1));
    }
    auto row_end(size_type i) -> row_iterator { return row_iterator(A::begin() + (i + 1)); }
    //! \brief Returns a const iterator to the first element after the end of the
    //! ith row.
    [[nodiscard]] auto crow_end(size_type i) const -> const_row_iterator
    {
        return const_row_iterator(A::begin() + (i + 1));
    }
    //! \brief Returns an iterator to the first element of the jth column.
    [[nodiscard]] auto col_begin(size_type /*unused*/ = 0) const -> const_column_iterator
    {
        return const_column_iterator(A::begin(), 1);
    }
    auto col_begin(size_type /*unused*/ = 0) -> column_iterator { return column_iterator(A::begin(), 1); }
    //! \brief Returns a const iterator to the first element of the jth column.
    [[nodiscard]] auto ccol_begin(size_type /*unused*/ = 0) const -> const_column_iterator
    {
        return const_column_iterator(A::begin(), 1);
    }
    //! \brief Returns an iterator to the first element after the end of the jth
    //! column.
    [[nodiscard]] auto col_end(size_type /*unused*/ = 0) const -> const_column_iterator
    {
        return const_column_iterator(A::end(), 1);
    }
    auto col_end(size_type /*unused*/ = 0) -> column_iterator { return column_iterator(A::end(), 1); }
    //! \brief Returns a const iterator to the first element after the end of the
    //! jth column.
    [[nodiscard]] auto ccol_end(size_type /*unused*/ = 0) const -> const_column_iterator
    {
        return const_column_iterator(A::end(), 1);
    }
    //! \brief Getters.
    [[nodiscard]] auto x() const -> typename A::value_type { return A::operator[](0); }
    [[nodiscard]] auto y() const -> typename A::value_type { return A::operator[](1); }
    [[nodiscard]] auto z() const -> typename A::value_type { return A::operator[](2); }
    [[nodiscard]] auto w() const -> typename A::value_type { return A::operator[](3); }
    auto x() -> typename A::value_type & { return A::operator[](0); }
    auto y() -> typename A::value_type & { return A::operator[](1); }
    auto z() -> typename A::value_type & { return A::operator[](2); }
    auto w() -> typename A::value_type & { return A::operator[](3); }
};

using size_type = iloj::misc::Array::size_type;
} // namespace Vector

namespace stack
{
template<typename T, Vector::size_type M>
using Vector = Vector::Interface<iloj::misc::stack::Array<T, M>>;

template<typename T>
using Vec2 = Vector<T, 2>;
template<typename T>
using Vec3 = Vector<T, 3>;
template<typename T>
using Vec4 = Vector<T, 4>;
template<typename T>
using Vec5 = Vector<T, 5>;
template<typename T>
using Vec6 = Vector<T, 6>;

//! \brief Returns the cross-product of a and b.
template<typename T, typename U>
auto cross(const Vec3<T> &a, const Vec3<U> &b) -> Vec3<decltype(T(0) * U(0))>
{
    Vec3<decltype(T(0) * U(0))> out;

    out[0] = a[1] * b[2] - a[2] * b[1];
    out[1] = a[2] * b[0] - a[0] * b[2];
    out[2] = a[0] * b[1] - a[1] * b[0];

    return out;
}

//! \brief Returns the triple-product of a, b and c (a . (b x c)).
template<typename T, typename U, typename V>
auto triple(const Vec3<T> &a, const Vec3<U> &b, const Vec3<V> &c) -> decltype(T(0) * U(0) * V(0))
{
    return dot(a, cross(b, c));
}

//! \brief Returns the solid angle captured by the 3 vertices given as
//! parameters
template<typename T, typename U, typename V>
auto solid(const Vec3<T> &a, const Vec3<U> &b, const Vec3<V> &c) -> double
{
    double na = norm(a);
    double nb = norm(b);
    double nc = norm(c);
    double out =
        2. * atan(std::abs(triple(a, b, c)) / (na * nb * nc + na * dot(b, c) + nb * dot(a, c) + nc * dot(a, b)));

    if (out < 0.)
    {
        return (out + M_PI);
    }
    {
        return out;
    }
}
} // namespace stack

namespace heap
{
template<typename T>
using Vector = Vector::Interface<iloj::misc::heap::Array<1, T>>;
}

namespace shallow
{
template<typename T>
using Vector = Vector::Interface<iloj::misc::shallow::Array<1, T>>;
}

// Additional definitions
using Vec2b = stack::Vec2<bool>;
using Vec2i = stack::Vec2<int>;
using Vec2u = stack::Vec2<unsigned>;
using Vec2f = stack::Vec2<float>;
using Vec2d = stack::Vec2<double>;
using Vec3b = stack::Vec3<bool>;
using Vec3i = stack::Vec3<int>;
using Vec3u = stack::Vec3<unsigned>;
using Vec3f = stack::Vec3<float>;
using Vec3d = stack::Vec3<double>;
using Vec4b = stack::Vec4<bool>;
using Vec4i = stack::Vec4<int>;
using Vec4u = stack::Vec4<unsigned>;
using Vec4f = stack::Vec4<float>;
using Vec4d = stack::Vec4<double>;
template<typename T>
using Vec = heap::Vector<T>;

//! \brief Dot product.
template<
    typename Iterator1,
    typename Iterator2,
    typename std::enable_if<std::is_floating_point<typename std::iterator_traits<Iterator1>::value_type>::value &&
                                std::is_floating_point<typename std::iterator_traits<Iterator2>::value_type>::value,
                            int>::type = 0>
auto dot_product(Iterator1 first1, Iterator1 last1, Iterator2 first2)
    -> decltype((typename std::iterator_traits<Iterator1>::value_type)(0) *
                (typename std::iterator_traits<Iterator2>::value_type)(0))
{
    return std::inner_product(first1, last1, first2, (typename std::iterator_traits<Iterator1>::value_type)(0));
}

template<
    typename Iterator1,
    typename Iterator2,
    typename std::enable_if<!std::is_floating_point<typename std::iterator_traits<Iterator1>::value_type>::value &&
                                !std::is_floating_point<typename std::iterator_traits<Iterator2>::value_type>::value,
                            int>::type = 0>
auto dot_product(Iterator1 first1, Iterator1 last1, Iterator2 first2)
    -> decltype((typename std::iterator_traits<Iterator1>::value_type)(0) *
                (typename std::iterator_traits<Iterator2>::value_type)(0))
{
    using T1 = typename std::iterator_traits<Iterator1>::value_type;
    using T2 = typename std::iterator_traits<Iterator2>::value_type;

    return std::inner_product(
        first1,
        last1,
        first2,
        (T1)(0),
        [](const T1 &v1, const T2 &v2) { return (v1 + v2); },
        [](const T1 &v1, const T2 &v2) { return (v1 * std::conj(v2)); });
}

template<typename V1, typename V2>
auto dot(const V1 &v1, const V2 &v2) -> decltype((typename V1::value_type)(0) * (typename V2::value_type)(0))
{
    return dot_product(v1.begin(), v1.end(), v2.begin());
}

//! \brief Returns ||v||**2.
template<typename V>
auto norm2(const V &v) -> decltype(std::abs(typename V::value_type(0)))
{
    return std::abs(dot(v, v));
}

//! \brief Returns ||v||.
template<typename V>
auto norm(const V &v) -> decltype(std::abs(typename V::value_type(0)))
{
    return sqrt(norm2(v));
}

//! \brief Returns ||v1 - v2||.
template<typename V1, typename V2>
auto distance(const V1 &v1, const V2 &v2) -> decltype((typename V1::value_type)(0) + (typename V2::value_type)(0))
{
    return sqrt(norm2(v1) + norm2(v2) -
                decltype((typename V1::value_type)(0) + (typename V2::value_type)(0))(2) * dot(v1, v2));
}

//! \brief Returns ||v||inf.
template<typename V>
auto norm_inf(const V &v) -> decltype(std::abs(typename V::value_type(0)))
{
    if (!v.empty())
    {
        return std::abs(
            *std::max_element(v.begin(), v.end(), [](auto v1, auto v2) { return std::abs(v1) < std::abs(v2); }));
    }

    return 0;
}

//! \brief Returns v / ||v|| and optionally ||v||.
template<typename V, typename U = decltype(std::abs(typename V::value_type(0)))>
auto unit(const V &v, U *n = nullptr) -> V
{
    U m = norm(v);
    if (n)
    {
        *n = m;
    }
    return v / m;
}

//! \brief Normalizes v and optionally returns ||v||.
template<typename V, typename U = decltype(std::abs(typename V::value_type(0)))>
auto normalize(V &v, U *n = nullptr) -> V &
{
    U m = norm(v);
    if (n)
    {
        *n = m;
    }
    v /= m;
    return v;
}
} // namespace iloj::math
