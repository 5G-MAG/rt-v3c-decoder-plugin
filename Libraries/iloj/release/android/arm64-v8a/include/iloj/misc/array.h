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

#include "memory.h"
#include "traits.h"
#include <algorithm>
#include <array>
#include <numeric>
#include <ostream>
#include <vector>

namespace iloj::misc
{
namespace Array
{
template<typename BASE,
         typename std::enable_if_t<std::is_same<typename std::iterator_traits<BASE>::iterator_category,
                                                std::random_access_iterator_tag>::value,
                                   int> = 0>
class dim_iterator
{
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = typename std::iterator_traits<BASE>::value_type;
    using difference_type = typename std::iterator_traits<BASE>::difference_type;
    using pointer = typename std::iterator_traits<BASE>::pointer;
    using reference = typename std::iterator_traits<BASE>::reference;

private:
    BASE m_iter;
    std::ptrdiff_t m_step = 0;

public:
    dim_iterator(BASE iter, std::ptrdiff_t s): m_iter(iter), m_step(s) {}
    ~dim_iterator() = default;
    dim_iterator(const dim_iterator &iter) = default;
    dim_iterator(dim_iterator &&iter) noexcept = default;
    [[nodiscard]] auto n() const -> std::ptrdiff_t { return m_step; }
    auto operator=(const dim_iterator &rhs) -> dim_iterator & = default;
    auto operator=(dim_iterator &&rhs) noexcept -> dim_iterator & = default;
    auto operator==(const dim_iterator &rhs) const -> bool { return m_iter == rhs.m_iter; }
    auto operator!=(const dim_iterator &rhs) const -> bool { return m_iter != rhs.m_iter; }
    auto operator*() const -> reference { return *m_iter; }
    auto operator->() const -> pointer { return m_iter.operator->(); }
    auto operator++() -> dim_iterator &
    {
        m_iter += m_step;
        return *this;
    }
    auto operator++(int) -> dim_iterator
    {
        dim_iterator out(*this);
        operator++();
        return out;
    }
    auto operator--() -> dim_iterator &
    {
        m_iter -= m_step;
        return *this;
    }
    auto operator--(int) -> dim_iterator
    {
        dim_iterator out(*this);
        operator--();
        return out;
    }
    auto operator+(std::ptrdiff_t a) const -> dim_iterator { return dim_iterator(m_iter + a * m_step, this->m_step); }
    auto operator+=(std::ptrdiff_t a) -> dim_iterator &
    {
        m_iter += (a * m_step);
        return *this;
    }
    auto operator-(const dim_iterator &iter) const -> std::ptrdiff_t { return (m_iter - iter.m_iter) / m_step; }
    auto operator-(std::ptrdiff_t a) const -> dim_iterator { return dim_iterator(m_iter - a * m_step, this->m_step); }
    auto operator-=(std::ptrdiff_t a) -> dim_iterator &
    {
        m_iter -= (a * m_step);
        return *this;
    }
    auto operator[](std::ptrdiff_t a) const -> reference { return m_iter[a * m_step]; }
    auto operator<(const dim_iterator &rhs) const -> bool { return m_iter < rhs.m_iter; }
    auto operator<=(const dim_iterator &rhs) const -> bool { return m_iter <= rhs.m_iter; }
    auto operator>(const dim_iterator &rhs) const -> bool { return m_iter > rhs.m_iter; }
    auto operator>=(const dim_iterator &rhs) const -> bool { return m_iter >= rhs.m_iter; }
    void swap(dim_iterator &a, dim_iterator &b) { std::swap(a, b); }
};

template<typename BASE>
auto operator+(std::ptrdiff_t a, const dim_iterator<BASE> &rhs) -> dim_iterator<BASE>
{
    return rhs + a;
}

using size_type = std::size_t;
has_type_macro(dim_iterator);
} // namespace Array

namespace stack
{
using size_type = Array::size_type;

namespace detail
{
template<size_type D, typename T, size_type... I>
struct Array
{
};

template<typename T, size_type M>
struct Array<1, T, M>
{
public:
    using iterator = T *;
    using const_iterator = const T *;

private:
    std::array<T, M> m_v{};

public:
    static constexpr auto size(size_type /*unused*/) -> size_type { return M; }
    //     static void sizes(size_type *iter) { *iter = M; }
    template<typename TUPLE>
    static void sizes(TUPLE &tuple, std::size_t pos)
    {
        tuple[pos] = M;
    }
    static constexpr auto size() -> size_type { return M; }
    static constexpr auto min_size() -> size_type { return M; }
    auto data() -> T * { return m_v.data(); }
    [[nodiscard]] auto data() const -> const T * { return m_v.data(); }
    template<size_type K>
    static constexpr auto offset(size_type i, size_type first = 0, size_type second = 0) -> size_type
    {
        return (i == K) ? second : first;
    }
    static constexpr auto step(size_type i) -> size_type { return (i != 0U) ? 1 : M; }
    static constexpr auto diag_step() -> size_type { return 1; }
    [[nodiscard]] auto get(size_type first) const -> T { return m_v[first]; }
    auto get(size_type first) -> T & { return m_v[first]; }
    auto begin() -> iterator { return m_v.data(); }
    [[nodiscard]] auto begin() const -> const_iterator { return m_v.data(); }
    auto end() -> iterator { return m_v.data() + size(); }
    [[nodiscard]] auto end() const -> const_iterator { return begin() + size(); }
};

template<size_type D, typename T, size_type M, size_type N, size_type... I>
struct Array<D, T, M, N, I...>
{
public:
    using iterator = typename Array<D - 1, T, N, I...>::iterator;
    using const_iterator = typename Array<D - 1, T, N, I...>::const_iterator;

private:
    std::array<Array<D - 1, T, N, I...>, M> m_v;

public:
    static constexpr auto size(size_type i) -> size_type
    {
        return (i != 0U) ? Array<D - 1, T, N, I...>::size(i - 1) : M;
    }
    template<typename TUPLE>
    static void sizes(TUPLE &tuple, std::size_t pos)
    {
        tuple[pos] = M;
        Array<D - 1, T, N, I...>::sizes(tuple, pos + 1);
    }
    static constexpr auto size() -> size_type { return M * Array<D - 1, T, N, I...>::size(); }
    static auto min_size() -> size_type { return (std::min)(M, Array<D - 1, T, N, I...>::min_size()); }
    auto data() -> T * { return m_v[0].data(); }
    [[nodiscard]] auto data() const -> const T * { return m_v[0].data(); }
    template<size_type K, typename... J>
    static constexpr auto offset(size_type i, size_type first, J... next) -> size_type
    {
        return (i == K) ? Array<D - 1, T, N, I...>::template offset<K>(i + 1, first, next...)
                        : first * Array<D - 1, T, N, I...>::size() +
                              Array<D - 1, T, N, I...>::template offset<K>(i + 1, next...);
    }
    static constexpr auto step(size_type i) -> size_type
    {
        return (i != 0U) ? Array<D - 1, T, N, I...>::step(i - 1) : M * Array<D - 1, T, N, I...>::step(i);
    }
    static constexpr auto diag_step() -> size_type { return step(1) + Array<D - 1, T, N, I...>::diag_step(); }
    template<typename... J>
    [[nodiscard]] auto get(size_type first, J... next) const -> T
    {
        return m_v[first].get(next...);
    }
    template<typename... J>
    auto get(size_type first, J... next) -> T &
    {
        return m_v[first].get(next...);
    }
    auto begin() -> iterator { return m_v.front().begin(); }
    [[nodiscard]] auto begin() const -> const_iterator { return m_v.front().begin(); }
    auto end() -> iterator { return begin() + size(); }
    [[nodiscard]] auto end() const -> const_iterator { return begin() + size(); }
};

} // namespace detail

//! \brief Multidimensional array (stack memory)
template<typename T, size_type... I>
class Array
{
public:
    using value_type = T;
    using reference = T &;
    using const_reference = const T &;
    using iterator = typename detail::Array<sizeof...(I), T, I...>::iterator;
    using const_iterator = typename detail::Array<sizeof...(I), T, I...>::const_iterator;
    using dim_iterator = typename iloj::misc::Array::dim_iterator<iterator>;
    using const_dim_iterator = typename iloj::misc::Array::dim_iterator<const_iterator>;
    using diag_iterator = dim_iterator;
    using const_diag_iterator = const_dim_iterator;
    using difference_type = std::ptrdiff_t;
    using size_type = stack::size_type;
    using container_type = Array<T, I...>;
    using tuple_type = std::array<size_type, sizeof...(I)>;
    template<typename U>
    using promoted_type = Array<decltype(T(0) * U(0)), I...>;

private:
    class Helper
    {
    private:
        tuple_type m_sizes;

    public:
        Helper() { detail::Array<sizeof...(I), T, I...>::sizes(m_sizes, 0); };
        [[nodiscard]] auto sizes() const -> const tuple_type & { return m_sizes; }
    };

private:
    static const Helper m_helper;
    detail::Array<sizeof...(I), T, I...> m_v;

public:
    //! \brief Default constructor.
    Array() = default;
    //! \brief Destructor.
    ~Array() = default;
    //! \brief Copy constructors.
    Array(const container_type &that) = default;
    explicit Array(T t) { std::fill(begin(), end(), t); }
    Array(std::initializer_list<T> v) { std::copy(v.begin(), v.begin() + size(), begin()); }
    template<typename OTHER, typename = typename OTHER::const_iterator>
    Array(const OTHER &that): Array()
    {
        std::transform(that.begin(), std::min(that.begin() + size(), that.end()), begin(), [](auto v) { return T(v); });
    }
    //! \brief Move constructor.
    Array(container_type &&that) noexcept = default;
    //! \brief Copy assignment.
    auto operator=(const container_type &that) -> Array & = default;
    auto operator=(T t) -> Array &
    {
        std::fill(begin(), end(), t);
        return *this;
    }
    auto operator=(std::initializer_list<T> v) -> Array &
    {
        std::copy(v.begin(), v.begin() + size(), begin());
        return *this;
    }
    template<typename OTHER, typename = typename OTHER::const_iterator>
    auto operator=(const OTHER &that) -> Array &
    {
        std::transform(that.begin(), std::min(that.begin() + size(), that.end()), begin(), [](auto v) { return T(v); });
        return *this;
    }
    //! \brief Move assignment.
    auto operator=(container_type &&that) noexcept -> Array & = default;
    //! \brief Equal operator.
    auto operator==(const container_type &that) const -> bool { return std::equal(begin(), end(), that.begin()); }
    //! \brief Different operator.
    auto operator!=(const container_type &that) const -> bool { return !std::equal(begin(), end(), that.begin()); }
    //! \brief Swap operator.
    void swap(container_type &that) { std::swap(m_v, that.m_v); }
    //! \brief Return true if resizable.
    [[nodiscard]] auto resizable() const -> bool { return false; }
    //! \brief Resize operator.
    void resize(const tuple_type & /*unused*/) {}
    //! \brief Reshape operator.
    void reshape(const tuple_type & /*unused*/) {}
    //! \brief Returns the array dimension.
    static constexpr auto dim() -> size_type { return sizeof...(I); }
    //! \brief Returns the array size along the i-th dimension.
    static constexpr auto size(size_type i) -> size_type { return detail::Array<sizeof...(I), T, I...>::size(i); }
    //! \brief Returns the array sizes.
    static auto sizes() -> const tuple_type & { return m_helper.sizes(); }
    //! \brief Returns the array total length
    static constexpr auto size() -> size_type { return detail::Array<sizeof...(I), T, I...>::size(); }
    //! \brief Returns the gap between 2 consecutive elements on the ith
    //! dimension.
    static constexpr auto step(size_type i) -> size_type { return detail::Array<sizeof...(I), T, I...>::step(i + 1); }
    //! \brief Returns true if the array is empty.
    static constexpr auto empty() -> bool { return (size() == 0); }
    //! \brief Data access, returns a pointer to the first element of the array.
    auto data() -> T * { return m_v.data(); }
    [[nodiscard]] auto data() const -> const T * { return m_v.data(); }
    //! \brief [] operator, returns the kth element of the array viewed as a one
    //! dimensional array.
    auto operator[](size_type k) const -> T { return *(begin() + k); }
    auto operator[](size_type k) -> T & { return *(begin() + k); }
    //! \brief Return the property of the array
    [[nodiscard]] auto getProperty() const -> int { return -1; }
    //! \brief Returns an iterator to the first element of the array.
    auto begin() -> iterator { return m_v.begin(); }
    [[nodiscard]] auto begin() const -> const_iterator { return m_v.begin(); }
    //! \brief Returns a const iterator to the first element of the array.
    [[nodiscard]] auto cbegin() const -> const_iterator { return begin(); }
    //! \brief Returns an iterator to the first element after the end of the
    //! array.
    auto end() -> iterator { return m_v.end(); }
    [[nodiscard]] auto end() const -> const_iterator { return m_v.end(); }
    //! \brief Returns a const iterator to the first element after the end of the
    //! array.
    [[nodiscard]] auto cend() const -> const_iterator { return m_v.end(); }
    //! \brief Returns an iterator along the Kth dimension to the first element of
    //! the hyperplane defined by next.
    template<size_type K, typename... J>
    [[nodiscard]] auto dim_begin(J... next) const -> const_dim_iterator
    {
        return const_dim_iterator(begin() + detail::Array<sizeof...(I), T, I...>::template offset<K>(0, next...),
                                  detail::Array<sizeof...(I), T, I...>::step(K + 1));
    }
    template<size_type K, typename... J>
    auto dim_begin(J... next) -> dim_iterator
    {
        return dim_iterator(begin() + detail::Array<sizeof...(I), T, I...>::template offset<K>(0, next...),
                            detail::Array<sizeof...(I), T, I...>::step(K + 1));
    }
    //! \brief Returns a const iterator along the Kth dimension to the first
    //! element of the hyperplane defined by next.
    template<size_type K, typename... J>
    auto cdim_begin(J... next) const -> const_dim_iterator
    {
        return cdim_begin(next...);
    }
    //! \brief Returns an iterator along the Kth dimension to the first element
    //! after the end of the hyperplane defined by next.
    template<size_type K, typename... J>
    [[nodiscard]] auto dim_end(J... next) const -> const_dim_iterator
    {
        return const_dim_iterator(begin() + detail::Array<sizeof...(I), T, I...>::template offset<K>(0, next...) +
                                      detail::Array<sizeof...(I), T, I...>::step(K),
                                  detail::Array<sizeof...(I), T, I...>::step(K + 1));
    }
    template<size_type K, typename... J>
    auto dim_end(J... next) -> dim_iterator
    {
        return dim_iterator(begin() + detail::Array<sizeof...(I), T, I...>::template offset<K>(0, next...) +
                                detail::Array<sizeof...(I), T, I...>::step(K),
                            detail::Array<sizeof...(I), T, I...>::step(K + 1));
    }
    //! \brief Returns a const iterator along the Kth dimension to the first
    //! element after the end of the hyperplane defined by next.
    template<size_type K, typename... J>
    auto cdim_end(J... next) const -> const_dim_iterator
    {
        return dim_end(next...);
    }
    //! \brief Returns an iterator to the first diagonal element.
    [[nodiscard]] auto diag_begin() const -> const_diag_iterator
    {
        return const_diag_iterator(begin(), detail::Array<sizeof...(I), T, I...>::diag_step());
    }
    auto diag_begin() -> diag_iterator
    {
        return diag_iterator(begin(), detail::Array<sizeof...(I), T, I...>::diag_step());
    }
    //! \brief Returns a const iterator to the first diagonal element.
    [[nodiscard]] auto cdiag_begin() const -> const_diag_iterator { return diag_begin(); }
    //! \brief Returns an iterator to the first element afer the last diagonal
    //! element.
    [[nodiscard]] auto diag_end() const -> const_diag_iterator
    {
        return const_diag_iterator(begin() + detail::Array<sizeof...(I), T, I...>::min_size() *
                                                 detail::Array<sizeof...(I), T, I...>::diag_step(),
                                   detail::Array<sizeof...(I), T, I...>::diag_step());
    }
    auto diag_end() -> diag_iterator
    {
        return diag_iterator(begin() + detail::Array<sizeof...(I), T, I...>::min_size() *
                                           detail::Array<sizeof...(I), T, I...>::diag_step(),
                             detail::Array<sizeof...(I), T, I...>::diag_step());
    }
    //! \brief Returns a const iterator to the first element afer the last
    //! diagonal element.
    [[nodiscard]] auto cdiag_end() const -> const_diag_iterator { return diag_end(); }
    //! \brief Returns m(i, j, k, ...)
    template<typename... J>
    auto operator()(J... idx) const -> T
    {
        return m_v.get(idx...);
    }
    template<typename... J>
    auto operator()(J... idx) -> T &
    {
        return m_v.get(idx...);
    }
    //! \brief Unary - operator.
    auto operator-() const -> container_type
    {
        container_type v;
        std::transform(begin(), end(), v.begin(), [](T x) { return -x; });
        return v;
    }
    //! \brief += scalar operator.
    auto operator+=(T v) -> container_type &
    {
        std::for_each(begin(), end(), [v](T &a) { a += v; });
        return *this;
    }
    //! \brief -= scalar operator.
    auto operator-=(T v) -> container_type &
    {
        std::for_each(begin(), end(), [v](T &a) { a -= v; });
        return *this;
    }
    //! \brief /= scalar operator.
    auto operator/=(T v) -> container_type &
    {
        std::for_each(begin(), end(), [v](T &a) { a /= v; });
        return *this;
    }
    //! \brief *= scalar operator.
    auto operator*=(T v) -> container_type &
    {
        std::for_each(begin(), end(), [v](T &a) { a *= v; });
        return *this;
    }
    //! \brief += operator.
    template<typename OTHER, typename = typename OTHER::const_iterator>
    auto operator+=(const OTHER &that) -> container_type &
    {
        std::transform(
            begin(), end(), that.begin(), begin(), [](T v1, typename OTHER::value_type v2) { return (v1 + v2); });
        return *this;
    }
    //! \brief += operator.
    template<typename OTHER, typename = typename OTHER::const_iterator>
    auto operator-=(const OTHER &that) -> container_type &
    {
        std::transform(
            begin(), end(), that.begin(), begin(), [](T v1, typename OTHER::value_type v2) { return (v1 - v2); });
        return *this;
    }
    //! \brief Return array filled with 0
    static auto zeros() -> container_type
    {
        container_type out;
        std::fill(out.begin(), out.end(), T{0});
        return out;
    }
    //! \brief Return array with diagonal filled with 1
    static auto eye() -> container_type
    {
        container_type out;
        std::fill(out.begin(), out.end(), T{0});
        std::fill(out.diag_begin(), out.diag_end(), T{1});
        return out;
    }
};

template<typename T, size_type... I>
const typename Array<T, I...>::Helper Array<T, I...>::m_helper;
} // namespace stack

namespace heap
{
using size_type = Array::size_type;

//! \brief Multidimensional array (heap memory)
template<size_type D, typename T>
class Array
{
public:
    using value_type = T;
    using reference = T &;
    using const_reference = const T &;
    using iterator = T *;
    using const_iterator = const T *;
    using dim_iterator = iloj::misc::Array::dim_iterator<iterator>;
    using const_dim_iterator = iloj::misc::Array::dim_iterator<const_iterator>;
    using diag_iterator = dim_iterator;
    using const_diag_iterator = const_dim_iterator;
    using difference_type = std::ptrdiff_t;
    using size_type = heap::size_type;
    using container_type = Array<D, T>;
    using tuple_type = std::array<size_type, D>;
    template<typename U>
    using promoted_type = Array<D, decltype(T{} * U{})>;
    using allocator_type = typename std::vector<T>::allocator_type;

private:
    std::array<size_type, D> m_size{};
    std::array<size_type, D + 1> m_step{};
    std::vector<T> m_v;
    int m_property = -1;

public:
    //! \brief Default constructor.
    Array()
    {
        m_size.fill(0);
        m_step.fill(0);
    }
    //! \brief Constructor with explicit size.
    explicit Array(const tuple_type &sz): Array() { this->resize(sz); }
    //! \brief Destructor.
    ~Array() = default;
    //! \brief Copy constructors.
    Array(const container_type &that) = default;
    Array(const tuple_type &sz, T v): Array()
    {
        this->resize(sz);
        std::fill(begin(), end(), v);
    }
    Array(const tuple_type &sz, std::initializer_list<T> v): Array()
    {
        this->resize(sz);
        m_v = v;
    }
    template<typename OTHER, typename = typename OTHER::dim_iterator>
    Array(const OTHER &that): Array()
    {
        tuple_type sz;
        std::copy(that.sizes().begin(), that.sizes().end(), sz.begin());
        std::fill(sz.begin() + that.sizes().size(), sz.end(), 1);
        this->resize(sz);
        std::transform(that.begin(), that.end(), begin(), [](auto v) { return T(v); });
        m_property = that.getProperty();
    }
    //! \brief Move constructor.
    Array(container_type &&that) noexcept
    {
        m_size = that.m_size;
        m_step = that.m_step;
        m_v = std::move(that.m_v);
        m_property = that.m_property;
        that.m_size.fill(0);
        that.m_step.fill(0);
        that.m_property = -1;
    }
    //! \brief Copy assignment.
    auto operator=(const container_type &that) -> Array & = default;
    template<typename OTHER, typename = typename OTHER::dim_iterator>
    auto operator=(const OTHER &that) -> Array &
    {
        tuple_type sz;
        std::copy(that.sizes().begin(), that.sizes().end(), sz.begin());
        std::fill(sz.begin() + that.sizes().size(), sz.end(), 1);
        this->resize(sz);
        std::transform(that.begin(), that.end(), begin(), [](auto v) { return T(v); });
        m_property = that.getProperty();
        return *this;
    }
    auto operator=(T v) -> Array &
    {
        std::fill(begin(), end(), v);
        return *this;
    }
    //! \brief Move assignment.
    auto operator=(container_type &&that) noexcept -> Array &
    {
        m_size = that.m_size;
        m_step = that.m_step;
        m_v = std::move(that.m_v);
        m_property = that.m_property;
        that.m_size.fill(0);
        that.m_step.fill(0);
        that.m_property = -1;
        return *this;
    }
    //! \brief Equal operator.
    auto operator==(const container_type &that) const -> bool
    {
        return (std::equal(m_size.begin(), m_size.end(), that.m_size.begin()) &&
                std::equal(begin(), end(), that.begin()));
    }
    //! \brief Different operator.
    auto operator!=(const container_type &that) const -> bool
    {
        return (!std::equal(m_size.begin(), m_size.end(), that.m_size.begin()) ||
                !std::equal(begin(), end(), that.begin()));
    }
    //! \brief Swap operator
    void swap(container_type &that)
    {
        std::swap(m_size, that.m_size);
        std::swap(m_step, that.m_step);
        m_v.swap(that.m_v);
        std::swap(m_property, that.m_property);
    }
    //! \brief Return true if resizable.
    [[nodiscard]] auto resizable() const -> bool { return true; }
    //! \brief Resize operator.
    void resize(const tuple_type &sz)
    {
        if (std::equal(m_size.begin(), m_size.end(), sz.begin()))
        {
            return;
        }

        // Dimensions
        std::copy(sz.begin(), sz.end(), m_size.begin());

        // Lengths
        size_type l = 1;
        m_step.back() = 1;
        std::transform(m_size.rbegin(), m_size.rend(), m_step.rbegin() + 1, [&l](size_type s) {
            l *= s;
            return l;
        });

        // Data
        m_v.resize(m_step.front());
    }
    //! \brief Reshape operator.
    void reshape(const tuple_type &sz)
    {
        if (std::equal(m_size.begin(), m_size.end(), sz.begin()))
        {
            return;
        }

        // Dimensions
        std::copy(sz.begin(), sz.end(), m_size.begin());

        // Lengths
        size_type l = 1;

        m_step.back() = 1;

        std::transform(m_size.rbegin(), m_size.rend(), m_step.rbegin() + 1, [&l](size_type s) {
            l *= s;
            return l;
        });
    }
    //! \brief Returns the array dimension.
    static constexpr auto dim() -> size_type { return D; }
    //! \brief Returns the array size along the i-th dimension.
    [[nodiscard]] auto size(size_type i) const -> size_type { return m_size[i]; }
    //! \brief Returns the array sizes.
    [[nodiscard]] auto sizes() const -> const tuple_type & { return m_size; }
    //! \brief Returns the array total length.
    [[nodiscard]] auto size() const -> size_type { return m_step.front(); }
    //! \brief Returns the gap between 2 consecutive elements on the ith
    //! dimension.
    [[nodiscard]] auto step(size_type i) const -> size_type { return m_step[i + 1]; }
    //! \brief Returns the array steps
    [[nodiscard]] auto steps() const -> const std::array<size_type, D + 1> & { return m_step; }
    //! \brief Returns true if the array is empty.
    [[nodiscard]] auto empty() const -> bool { return (size() == 0); }
    //! \brief Data access, returns a pointer to the first element of the array.
    auto data() -> T * { return m_v.data(); }
    [[nodiscard]] auto data() const -> const T * { return m_v.data(); }
    //! \brief [] operator, returns the kth element of the array viewed as a one
    //! dimensional array.
    auto operator[](size_type k) const -> T { return m_v[k]; }
    auto operator[](size_type k) -> T & { return m_v[k]; }
    //! \brief Return the property of the array
    [[nodiscard]] auto getProperty() const -> int { return m_property; }
    // \brief Set the property of the array
    void setProperty(int v) { m_property = v; }
    //! \brief Returns an iterator to the first element of the array.
    auto begin() -> iterator { return m_v.data(); }
    [[nodiscard]] auto begin() const -> const_iterator { return m_v.data(); }
    //! \brief Returns a const iterator to the first element of the array.
    [[nodiscard]] auto cbegin() const -> const_iterator { return begin(); }
    //! \brief Returns an iterator to the first element after the end of the
    //! array.
    auto end() -> iterator { return begin() + size(); }
    [[nodiscard]] auto end() const -> const_iterator { return begin() + size(); }
    //! \brief Returns a const iterator to the first element after the end of the
    //! array.
    [[nodiscard]] auto cend() const -> const_iterator { return end(); }
    //! \brief Returns an iterator along the Kth dimension to the first element of
    //! the hyperplane defined by next.
    template<size_type K, typename... I>
    [[nodiscard]] [[nodiscard]] auto dim_begin(I... next) const -> const_dim_iterator
    {
        return const_dim_iterator(begin() + offset<K>(0, next...), m_step[K + 1]);
    }
    template<size_type K, typename... I>
    auto dim_begin(I... next) -> dim_iterator
    {
        return dim_iterator(begin() + offset<K>(0, next...), m_step[K + 1]);
    }
    //! \brief Returns a const iterator along the Kth dimension to the first
    //! element of the hyperplane defined by next.
    template<size_type K, typename... I>
    auto cdim_begin(I... next) const -> const_dim_iterator
    {
        return dim_begin(next...);
    }
    //! \brief Returns an iterator along the Kth dimension to the first element
    //! after the end of the hyperplane defined by next.
    template<size_type K, typename... I>
    [[nodiscard]] auto dim_end(I... next) const -> const_dim_iterator
    {
        return const_dim_iterator(begin() + offset<K>(0, next...) + m_step[K], m_step[K + 1]);
    }
    template<size_type K, typename... I>
    auto dim_end(I... next) -> dim_iterator
    {
        return dim_iterator(begin() + offset<K>(0, next...) + m_step[K], m_step[K + 1]);
    }
    //! \brief Returns a const iterator along the Kth dimension to the first
    //! element after the end of the hyperplane defined by next.
    template<size_type K, typename... I>
    auto cdim_end(I... next) const -> const_dim_iterator
    {
        return dim_end(next...);
    }
    //! \brief Returns an iterator to the first diagonal element.
    [[nodiscard]] auto diag_begin() const -> const_diag_iterator
    {
        return const_diag_iterator(begin(), std::accumulate(m_step.begin() + 1, m_step.end(), size_type(0)));
    }
    auto diag_begin() -> diag_iterator
    {
        return diag_iterator(begin(), std::accumulate(m_step.begin() + 1, m_step.end(), size_type(0)));
    }
    //! \brief Returns a const iterator to the first diagonal element.
    [[nodiscard]] auto cdiag_begin() const -> const_diag_iterator
    {
        return const_diag_iterator(begin(), std::accumulate(m_step.begin() + 1, m_step.end(), size_type(0)));
    }
    //! \brief Returns an iterator to the first element afer the last diagonal
    //! element.
    [[nodiscard]] auto diag_end() const -> const_diag_iterator
    {
        size_type d = std::accumulate(m_step.begin() + 1, m_step.end(), size_type(0));
        return const_diag_iterator(begin() + *std::min_element(m_size.begin(), m_size.end()) * d, d);
    }
    auto diag_end() -> diag_iterator
    {
        size_type d = std::accumulate(m_step.begin() + 1, m_step.end(), size_type(0));
        return diag_iterator(begin() + *std::min_element(m_size.begin(), m_size.end()) * d, d);
    }
    //! \brief Returns a const iterator to the first element afer the last
    //! diagonal element.
    [[nodiscard]] auto cdiag_end() const -> const_diag_iterator { return cdiag_end(); }
    //! \brief Returns m(i, j, k, ...)
    template<typename... I>
    auto operator()(size_type first, I... next) const -> T
    {
        return m_v[pos(1, first, next...)];
    }
    template<typename... I>
    auto operator()(size_type first, I... next) -> T &
    {
        return m_v[pos(1, first, next...)];
    }
    //! \brief Returns index from offset
    [[nodiscard]] auto index(size_type offset) const -> tuple_type
    {
        tuple_type idx{};

        for (size_type i = 0; i < D; i++)
        {
            idx[i] = offset / m_step[i + 1];
            offset -= (idx[i] * m_step[i + 1]);
        }

        return idx;
    }
    //! \brief Returns distance of m(i, j, k, ...) from m(0, 0, 0, ...)
    template<typename... I>
    auto distance(size_type first, I... next) const -> size_type
    {
        return pos(1, first, next...);
    }
    //! \brief Unary - operator.
    auto operator-() const -> container_type
    {
        container_type v(sizes());
        std::transform(begin(), end(), v.begin(), [](T x) { return -x; });
        return v;
    }
    //! \brief += scalar operator.
    auto operator+=(T v) -> container_type &
    {
        std::for_each(begin(), end(), [v](T &a) { a += v; });
        return *this;
    }
    //! \brief -= scalar operator.
    auto operator-=(T v) -> container_type &
    {
        std::for_each(begin(), end(), [v](T &a) { a -= v; });
        return *this;
    }
    //! \brief /= scalar operator.
    auto operator/=(T v) -> container_type &
    {
        std::for_each(begin(), end(), [v](T &a) { a /= v; });
        return *this;
    }
    //! \brief *= scalar operator.
    auto operator*=(T v) -> container_type &
    {
        std::for_each(begin(), end(), [v](T &a) { a *= v; });
        return *this;
    }
    //! \brief += operator.
    template<typename OTHER, typename = typename OTHER::const_iterator>
    auto operator+=(const OTHER &that) -> container_type &
    {
        std::transform(
            begin(), end(), that.begin(), begin(), [](T v1, typename OTHER::value_type v2) { return (v1 + v2); });
        return *this;
    }
    //! \brief -= operator.
    template<typename OTHER, typename = typename OTHER::const_iterator>
    auto operator-=(const OTHER &that) -> container_type &
    {
        std::transform(
            begin(), end(), that.begin(), begin(), [](T v1, typename OTHER::value_type v2) { return (v1 - v2); });
        return *this;
    }
    //! \brief Return array filled with 0
    static auto zeros(const tuple_type &sz) -> container_type
    {
        container_type out(sz);
        std::fill(out.begin(), out.end(), 0);
        return out;
    }
    //! \brief Return array with diagonal filled with 1
    static auto eye(const tuple_type &sz) -> container_type
    {
        container_type out(sz);

        std::fill(out.begin(), out.end(), T{0});
        std::fill(out.diag_begin(), out.diag_end(), T{1});

        return out;
    }

private:
    template<size_type K>
    [[nodiscard]] auto offset(size_type i, size_type first = 0) const -> size_type
    {
        return (i == K) ? first : first * m_step[i + 1];
    }
    template<size_type K, typename... I>
    [[nodiscard]] [[nodiscard]] auto offset(size_type i, size_type first, I... next) const -> size_type
    {
        return (i == K) ? offset<K>(i + 1, first, next...) : first * m_step[i + 1] + offset<K>(i + 1, next...);
    }
    [[nodiscard]] auto pos(size_type /*unused*/, size_type first) const -> size_type { return first; }
    template<typename... I>
    [[nodiscard]] [[nodiscard]] auto pos(size_type i, size_type first, I... next) const -> size_type
    {
        return first * m_step[i] + pos(i + 1, next...);
    }
};
} // namespace heap

namespace shallow
{
using size_type = Array::size_type;

//! \brief Multidimensional array (shallow / no-copy memory)
template<size_type D, typename T>
class Array
{
public:
    using value_type = T;
    using reference = T &;
    using const_reference = const T &;
    using iterator = T *;
    using const_iterator = const T *;
    using dim_iterator = typename iloj::misc::Array::dim_iterator<iterator>;
    using const_dim_iterator = typename iloj::misc::Array::dim_iterator<const_iterator>;
    using diag_iterator = dim_iterator;
    using const_diag_iterator = const_dim_iterator;
    using difference_type = std::ptrdiff_t;
    using size_type = shallow::size_type;
    using container_type = Array<D, T>;
    using tuple_type = std::array<size_type, D>;
    template<typename U>
    using promoted_type = Array<D, decltype(T(0) * U(0))>;

private:
    std::array<size_type, D> m_size{};
    std::array<size_type, D> m_lda{};
    std::array<size_type, D + 1> m_step{};
    T *m_v = nullptr;
    int m_property = -1;

public:
    //! \brief Default constructors.
    Array()
    {
        m_size.fill(0);
        m_lda.fill(0);
        m_step.fill(0);
    }
    //! \brief Destructor.
    ~Array() = default;
    //! \brief Copy constructors.
    Array(const tuple_type &sz, T *src, const tuple_type &lda = {}, const tuple_type &gap = {}): Array()
    {
        this->reshape(sz, lda, gap);
        m_v = src;
    }
    Array(const container_type &that) = default;
    template<typename OTHER,
             typename = typename OTHER::dim_iterator,
             typename std::enable_if_t<std::is_same<T, typename OTHER::value_type>::value, int> = 0>
    Array(const OTHER &that): Array()
    {
        tuple_type sz;
        std::copy(that.sizes().begin(), that.sizes().end(), sz.begin());
        std::fill(sz.begin() + that.sizes().size(), sz.end(), 1);
        this->reshape(sz);
        m_v = (T *) that.data();
        m_property = that.getProperty();
    }
    //! \brief Move constructor.
    Array(container_type &&that) noexcept
    {
        m_size = that.m_size;
        m_lda = that.m_lda;
        m_step = that.m_step;
        m_v = std::move(that.m_v);
        m_property = that.m_property;
        that.m_size.fill(0);
        that.m_lda.fill(0);
        that.m_step.fill(0);
        that.m_v = nullptr;
        that.m_property = -1;
    }
    //! \brief Copy assignment.
    auto operator=(const container_type &that) -> Array & = default;
    template<typename OTHER, typename = typename OTHER::dim_iterator>
    auto operator=(const OTHER &that) -> Array &
    {
        if (size() == that.size())
        {
            tuple_type sz;

            std::copy(that.sizes().begin(), that.sizes().end(), sz.begin());
            std::fill(sz.begin() + that.sizes().size(), sz.end(), 1);

            this->reshape(sz);

            std::transform(that.begin(), that.end(), begin(), [](auto v) { return T(v); });

            m_property = that.getProperty();
        }
        return *this;
    }
    auto operator=(T v) -> Array &
    {
        std::fill(begin(), end(), v);
        return *this;
    }
    //! \brief Move assignment.
    auto operator=(container_type &&that) noexcept -> Array &
    {
        m_size = that.m_size;
        m_lda = that.m_lda;
        m_step = that.m_step;
        m_v = that.m_v;
        m_property = that.m_property;
        that.m_size.fill(0);
        that.m_lda.fill(0);
        that.m_step.fill(0);
        that.m_v = nullptr;
        that.m_property = -1;
        return *this;
    }
    //! \brief Equal operator.
    auto operator==(const container_type &that) const -> bool
    {
        return (std::equal(m_size.begin(), m_size.end(), that.m_size.begin()) &&
                std::equal(begin(), end(), that.begin()));
    }
    //! \brief Different operator.
    auto operator!=(const container_type &that) const -> bool
    {
        return (!std::equal(m_size.begin(), m_size.end(), that.m_size.begin()) ||
                !std::equal(begin(), end(), that.begin()));
    }
    //! \brief Swap operator
    void swap(container_type &that)
    {
        std::swap(m_size, that.m_size);
        std::swap(m_lda, that.m_lda);
        std::swap(m_step, that.m_step);
        std::swap(m_v, that.m_v);
        std::swap(m_property, that.m_property);
    }
    //! \brief Return true if resizable.
    [[nodiscard]] auto resizable() const -> bool { return false; }
    //! \brief Resize operator.
    void resize(const tuple_type & /*unused*/) {}
    //! \brief Reshape operator.
    void reshape(const tuple_type &sz, const tuple_type & /* lda */ = {}, const tuple_type & /* gap */ = {})
    {
#if 0
        m_size = sz;
        m_lda = (lda.front() == 0) ? sz : lda;

        if(gap.front() == 0)
        {
            std::fill(m_gap.begin(), m_gap.end(), 1);
        }

        size_type l = 1;

        m_step.back() = 1;

        std::transform(m_size.rbegin(), m_size.rend(), m_step.rbegin() + 1, [&l](size_type s) {
            l *= s;
            return l;
        });
#endif

        if (std::equal(m_size.begin(), m_size.end(), sz.begin()))
        {
            return;
        }

        // Dimensions
        std::copy(sz.begin(), sz.end(), m_size.begin());

        // Lengths
        size_type l = 1;

        m_step.back() = 1;

        std::transform(m_size.rbegin(), m_size.rend(), m_step.rbegin() + 1, [&l](size_type s) {
            l *= s;
            return l;
        });
    }
    //! \brief Returns the array dimension.
    static constexpr auto dim() -> size_type { return D; }
    //! \brief Returns the array size along the i-th dimension.
    [[nodiscard]] auto size(size_type i) const -> size_type { return m_size[i]; }
    //! \brief Returns the array sizes.
    [[nodiscard]] auto sizes() const -> const tuple_type & { return m_size; }
    //! \brief Returns the array total length.
    [[nodiscard]] auto size() const -> size_type { return m_step.front(); }
    //! \brief Returns the gap between 2 consecutive elements on the ith
    //! dimension.
    [[nodiscard]] auto step(size_type i) const -> size_type { return m_step[i + 1]; }
    //! \brief Returns true if the array is empty.
    [[nodiscard]] auto empty() const -> bool { return (size() == 0); }
    //! \brief Data access, returns a pointer to the first element of the array.
    auto data() -> T * { return m_v; }
    [[nodiscard]] auto data() const -> const T * { return m_v; }
    //! \brief [] operator, returns the kth element of the array viewed as a one
    //! dimensional array.
    auto operator[](int k) const -> T { return m_v[k]; }
    auto operator[](int k) -> T & { return m_v[k]; }
    //! \brief Return the property of the array
    [[nodiscard]] auto getProperty() const -> int { return m_property; }
    // \brief Set the property of the array
    void setProperty(int v) { m_property = v; }
    //! \brief Returns an iterator to the first element of the array.
    auto begin() -> iterator { return m_v; }                           // TODO
    [[nodiscard]] auto begin() const -> const_iterator { return m_v; } // TODO
    //! \brief Returns a const iterator to the first element of the array.
    [[nodiscard]] auto cbegin() const -> const_iterator { return begin(); } // TODO
    //! \brief Returns an iterator to the first element after the end of the
    //! array.
    auto end() -> iterator { return m_v + size(); }                           // TODO
    [[nodiscard]] auto end() const -> const_iterator { return m_v + size(); } // TODO
    //! \brief Returns a const iterator to the first element after the end of the
    //! array.
    [[nodiscard]] auto cend() const -> const_iterator { return m_v + size(); } // TODO
    //! \brief Returns an iterator along the Kth dimension to the first element of
    //! the hyperplane defined by next.
    template<size_type K, typename... I>
    [[nodiscard]] [[nodiscard]] auto dim_begin(I... next) const -> const_dim_iterator
    {
        return const_dim_iterator(m_v + offset<K>(0, next...), m_step[K + 1]);
    }
    template<size_type K, typename... I>
    auto dim_begin(I... next) -> dim_iterator
    {
        return dim_iterator(m_v + offset<K>(0, next...), m_step[K + 1]);
    }
    //! \brief Returns a const iterator along the Kth dimension to the first
    //! element of the hyperplane defined by next.
    template<size_type K, typename... I>
    auto cdim_begin(I... next) const -> const_dim_iterator
    {
        return const_dim_iterator(m_v + offset<K>(0, next...), m_step[K + 1]);
    }
    //! \brief Returns an iterator along the Kth dimension to the first element
    //! after the end of the hyperplane defined by next.
    template<size_type K, typename... I>
    [[nodiscard]] [[nodiscard]] auto dim_end(I... next) const -> const_dim_iterator
    {
        return const_dim_iterator(m_v + offset<K>(0, next...) + m_step[K], m_step[K + 1]);
    }
    template<size_type K, typename... I>
    auto dim_end(I... next) -> dim_iterator
    {
        return dim_iterator(m_v + offset<K>(0, next...) + m_step[K], m_step[K + 1]);
    }
    //! \brief Returns a const iterator along the Kth dimension to the first
    //! element after the end of the hyperplane defined by next.
    template<size_type K, typename... I>
    auto cdim_end(I... next) const -> const_dim_iterator
    {
        return const_dim_iterator(m_v + offset<K>(0, next...) + m_step[K], m_step[K + 1]);
    }
    //! \brief Returns an iterator to the first diagonal element.
    [[nodiscard]] auto diag_begin() const -> const_diag_iterator
    {
        return const_diag_iterator(m_v, std::accumulate(m_step.begin() + 1, m_step.end(), size_type(0)));
    }
    auto diag_begin() -> diag_iterator
    {
        return diag_iterator(m_v, std::accumulate(m_step.begin() + 1, m_step.end(), size_type(0)));
    }
    //! \brief Returns a const iterator to the first diagonal element.
    [[nodiscard]] auto cdiag_begin() const -> const_diag_iterator
    {
        return const_diag_iterator(m_v, std::accumulate(m_step.begin() + 1, m_step.end(), size_type(0)));
    }
    //! \brief Returns an iterator to the first element afer the last diagonal
    //! element.
    [[nodiscard]] auto diag_end() const -> const_diag_iterator
    {
        size_type d = std::accumulate(m_step.begin() + 1, m_step.end(), size_type(0));
        return const_diag_iterator(m_v + *std::min_element(m_size.begin(), m_size.end()) * d, d);
    }
    auto diag_end() -> diag_iterator
    {
        size_type d = std::accumulate(m_step.begin() + 1, m_step.end(), size_type(0));
        return diag_iterator(m_v + *std::min_element(m_size.begin(), m_size.end()) * d, d);
    }
    //! \brief Returns a const iterator to the first element afer the last
    //! diagonal element.
    [[nodiscard]] auto cdiag_end() const -> const_diag_iterator { return diag_end(); }
    //! \brief Returns m(i, j, k, ...)
    template<typename... I>
    auto operator()(size_type first, I... next) const -> T
    {
        return m_v[pos(1, first, next...)];
    }
    template<typename... I>
    auto operator()(size_type first, I... next) -> T &
    {
        return m_v[pos(1, first, next...)];
    }
    //! \brief Returns index from offset
    [[nodiscard]] auto index(size_type offset) const -> tuple_type
    {
        tuple_type idx{};

        for (std::size_t i = 0; i < D; i++)
        {
            idx[i] = offset / m_step[i + 1];
            offset -= (idx[i] * m_step[i + 1]);
        }

        return idx;
    }
    //! \brief Returns distance of m(i, j, k, ...) from m(0, 0, 0, ...)
    template<typename... I>
    auto distance(size_type first, I... next) const -> size_type
    {
        return pos(1, first, next...);
    }
    //! \brief += scalar operator.
    auto operator+=(T v) -> container_type &
    {
        std::for_each(begin(), end(), [v](T &a) { a += v; });
        return *this;
    }
    //! \brief -= scalar operator.
    auto operator-=(T v) -> container_type &
    {
        std::for_each(begin(), end(), [v](T &a) { a -= v; });
        return *this;
    }
    //! \brief /= scalar operator.
    auto operator/=(T v) -> container_type &
    {
        std::for_each(begin(), end(), [v](T &a) { a /= v; });
        return *this;
    }
    //! \brief *= scalar operator.
    auto operator*=(T v) -> container_type &
    {
        std::for_each(begin(), end(), [v](T &a) { a *= v; });
        return *this;
    }
    //! \brief += operator.
    template<typename OTHER, typename = typename OTHER::const_iterator>
    auto operator+=(const OTHER &that) -> container_type &
    {
        std::transform(
            begin(), end(), that.begin(), begin(), [](T v1, typename OTHER::value_type v2) { return (v1 + v2); });
        return *this;
    }
    //! \brief -= operator.
    template<typename OTHER, typename = typename OTHER::const_iterator>
    auto operator-=(const OTHER &that) -> container_type &
    {
        std::transform(
            begin(), end(), that.begin(), begin(), [](T v1, typename OTHER::value_type v2) { return (v1 - v2); });
        return *this;
    }

private:
    template<size_type K>
    [[nodiscard]] auto offset(size_type i, size_type first = 0) const -> size_type
    {
        return (i == K) ? first : first * m_step[i + 1];
    }
    template<size_type K, typename... I>
    [[nodiscard]] [[nodiscard]] auto offset(size_type i, size_type first, I... next) const -> size_type
    {
        return (i == K) ? offset<K>(i + 1, first, next...) : first * m_step[i + 1] + offset<K>(i + 1, next...);
    }
    [[nodiscard]] auto pos(size_type /*unused*/, size_type first) const -> size_type { return first; }
    template<typename... I>
    [[nodiscard]] [[nodiscard]] [[nodiscard]] auto pos(size_type i, size_type first, I... next) const -> size_type
    {
        return first * m_step[i] + pos(i + 1, next...);
    }
};
} // namespace shallow

//! \brief Return true if a1 and a2 have the same size.
template<typename A1, typename A2>
auto same_size(const A1 &a1, const A2 &a2) -> bool
{
    if (a1.dim() != a2.dim())
    {
        return false;
    }

    for (typename A1::size_type i = 0; i < a1.dim(); i++)
    {
        if (a1.size(i) != a2.size(i))
        {
            return false;
        }
    }

    return true;
}
} // namespace iloj::misc

//! \brief Send the array a to the stream os.
template<typename A, std::enable_if_t<iloj::misc::Array::has_dim_iterator<A>::value, int> = 0>
auto operator<<(std::ostream &os, const A &a) -> std::ostream &
{
    typename A::size_type step = a.size(a.dim() - 1);

    for (auto iter = a.begin(); iter != a.end(); iter += step)
    {
        std::for_each(iter, iter + step, [&os](typename A::value_type v) { os << v << " "; });
        if ((iter + step) != a.end())
        {
            os << "\n";
        }
    }

    return os;
}

//! \brief Load the array a from the stream is.
template<typename A, std::enable_if_t<iloj::misc::Array::has_dim_iterator<A>::value, int> = 0>
auto operator>>(std::istream &is, A &a) -> std::istream &
{
    for (auto &e : a)
    {
        is >> e;
    }

    return is;
}

//! \brief Send the array a to the binary stream os.
template<typename A, std::enable_if_t<iloj::misc::Array::has_dim_iterator<A>::value, int> = 0>
auto operator<<(iloj::misc::OutputStream &os, const A &a) -> iloj::misc::OutputStream &
{
    if (a.resizable())
    {
        os << a.sizes();
    }

    for (const auto &e : a)
    {
        os << e;
    }

    return os;
}

//! \brief Load the array a from the binary stream is.
template<typename A, std::enable_if_t<iloj::misc::Array::has_dim_iterator<A>::value, int> = 0>
auto operator>>(iloj::misc::InputStream &is, A &a) -> iloj::misc::InputStream &
{
    if (a.resizable())
    {
        a.resize(is.read<typename A::tuple_type>());
    }

    for (auto &e : a)
    {
        is >> e;
    }

    return is;
}

//! \brief array/scalar + operator.
namespace iloj::misc::detail
{
template<typename A1,
         typename U,
         typename A2,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::is_numeric<U>::value, int> = 0,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A2>::value, int> = 0>
void add(const A1 &m, U u, A2 &out)
{
    out.resize(m.sizes());
    std::transform(
        m.begin(), m.end(), out.begin(), [u](typename A1::value_type v) -> typename A2::value_type { return (v + u); });
}

template<typename A1,
         typename U,
         typename A2,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::is_numeric<U>::value, int> = 0,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A2>::value, int> = 0>
void add(U u, const A1 &m, A2 &out)
{
    out.resize(m.sizes());
    std::transform(
        m.begin(), m.end(), out.begin(), [u](typename A1::value_type v) -> typename A2::value_type { return (u + v); });
}
} // namespace iloj::misc::detail

template<typename A1,
         typename U,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::is_numeric<U>::value, int> = 0>
auto operator+(const A1 &m, U u) -> typename A1::template promoted_type<U>
{
    typename A1::template promoted_type<U> out;
    iloj::misc::detail::add(m, u, out);
    return out;
}

template<typename A1,
         typename U,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::is_numeric<U>::value, int> = 0>
auto operator+(U u, const A1 &m) -> typename A1::template promoted_type<U>
{
    typename A1::template promoted_type<U> out;
    iloj::misc::detail::add(u, m, out);
    return out;
}

//! \brief array/scalar - operator.
namespace iloj::misc::detail
{
template<typename A1,
         typename U,
         typename A2,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::is_numeric<U>::value, int> = 0,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A2>::value, int> = 0>
void sub(const A1 &m, U u, A2 &out)
{
    out.resize(m.sizes());
    std::transform(
        m.begin(), m.end(), out.begin(), [u](typename A1::value_type v) -> typename A2::value_type { return (v - u); });
}

template<typename A1,
         typename U,
         typename A2,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::is_numeric<U>::value, int> = 0,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A2>::value, int> = 0>
void sub(U u, const A1 &m, A2 &out)
{
    out.resize(m.sizes());
    std::transform(
        m.begin(), m.end(), out.begin(), [u](typename A1::value_type v) -> typename A2::value_type { return (u - v); });
}
} // namespace iloj::misc::detail

template<typename A1,
         typename U,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::is_numeric<U>::value, int> = 0>
auto operator-(const A1 &m, U u) -> typename A1::template promoted_type<U>
{
    typename A1::template promoted_type<U> out;
    iloj::misc::detail::sub(m, u, out);
    return out;
}

template<typename A1,
         typename U,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::is_numeric<U>::value, int> = 0>
auto operator-(U u, const A1 &m) -> typename A1::template promoted_type<U>
{
    typename A1::template promoted_type<U> out;
    iloj::misc::detail::sub(u, m, out);
    return out;
}

//! \brief array/scalar * operator.
namespace iloj::misc::detail
{
template<typename A1,
         typename U,
         typename A2,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::is_numeric<U>::value, int> = 0,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A2>::value, int> = 0>
void mult(const A1 &m, U u, A2 &out)
{
    out.resize(m.sizes());
    std::transform(
        m.begin(), m.end(), out.begin(), [u](typename A1::value_type v) -> typename A2::value_type { return (v * u); });
}

template<typename A1,
         typename U,
         typename A2,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::is_numeric<U>::value, int> = 0,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A2>::value, int> = 0>
void mult(U u, const A1 &m, A2 &out)
{
    out.resize(m.sizes());
    std::transform(
        m.begin(), m.end(), out.begin(), [u](typename A1::value_type v) -> typename A2::value_type { return (u * v); });
}
} // namespace iloj::misc::detail

template<typename A1,
         typename U,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::is_numeric<U>::value, int> = 0>
auto operator*(const A1 &m, U u) -> typename A1::template promoted_type<U>
{
    typename A1::template promoted_type<U> out;
    iloj::misc::detail::mult(m, u, out);
    return out;
}

template<typename A1,
         typename U,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::is_numeric<U>::value, int> = 0>
auto operator*(U u, const A1 &m) -> typename A1::template promoted_type<U>
{
    typename A1::template promoted_type<U> out;
    iloj::misc::detail::mult(u, m, out);
    return out;
}

//! \brief array/scalar / operator.
namespace iloj::misc::detail
{
template<typename A1,
         typename U,
         typename A2,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::is_numeric<U>::value, int> = 0,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A2>::value, int> = 0>
void div(const A1 &m, U u, A2 &out)
{
    out.resize(m.sizes());
    std::transform(
        m.begin(), m.end(), out.begin(), [u](typename A1::value_type v) -> typename A2::value_type { return (v / u); });
}
} // namespace iloj::misc::detail

namespace iloj
{
template<typename A1,
         typename U,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::is_numeric<U>::value, int> = 0>
auto operator/(const A1 &m, U u) -> typename A1::template promoted_type<U>
{
    typename A1::template promoted_type<U> out;
    iloj::misc::detail::div(m, u, out);
    return out;
}
} // namespace iloj

//! \brief array/array + operator.
namespace iloj::misc::detail
{
template<typename A1,
         typename A2,
         typename A3,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A2>::value, int> = 0,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A3>::value, int> = 0>
void add(const A1 &m1, const A2 &m2, A3 &out)
{
    out.resize(m1.sizes());
    std::transform(m1.begin(),
                   m1.end(),
                   m2.begin(),
                   out.begin(),
                   [](typename A1::value_type v1, typename A2::value_type v2) ->
                   typename A3::value_type { return v1 + v2; });
}
} // namespace iloj::misc::detail

template<typename A1,
         typename A2,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A2>::value, int> = 0>
auto operator+(const A1 &m1, const A2 &m2) -> typename A1::template promoted_type<typename A2::value_type>
{
    typename A1::template promoted_type<typename A2::value_type> out;
    iloj::misc::detail::add(m1, m2, out);
    return out;
}

//! \brief array/array - operator.
namespace iloj::misc::detail
{
template<typename A1,
         typename A2,
         typename A3,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A2>::value, int> = 0,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A3>::value, int> = 0>
void sub(const A1 &m1, const A2 &m2, A3 &out)
{
    out.resize(m1.sizes());
    std::transform(m1.begin(),
                   m1.end(),
                   m2.begin(),
                   out.begin(),
                   [](typename A1::value_type v1, typename A2::value_type v2) ->
                   typename A3::value_type { return v1 - v2; });
}
} // namespace iloj::misc::detail

template<typename A1,
         typename A2,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A2>::value, int> = 0>
auto operator-(const A1 &m1, const A2 &m2) -> typename A1::template promoted_type<typename A2::value_type>
{
    typename A1::template promoted_type<typename A2::value_type> out;
    iloj::misc::detail::sub(m1, m2, out);
    return out;
}

//! \brief Element-by-element multiplication operator.
template<typename A1,
         typename A2,
         typename A3,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A2>::value, int> = 0,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A3>::value, int> = 0>
void mult(const A1 &m1, const A2 &m2, A3 &out)
{
    out.resize(m1.sizes());
    std::transform(m1.begin(),
                   m1.end(),
                   m2.begin(),
                   out.begin(),
                   [](typename A1::value_type v1, typename A2::value_type v2) ->
                   typename A3::value_type { return v1 * v2; });
}

template<typename A1,
         typename A2,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A2>::value, int> = 0>
auto mult(const A1 &m1, const A2 &m2) -> typename A1::template promoted_type<typename A2::value_type>
{
    typename A1::template promoted_type<typename A2::value_type> out;
    mult(m1, m2, out);
    return out;
}

//! \brief Element-by-element division operator.
namespace iloj::misc::detail
{
template<typename A1,
         typename A2,
         typename A3,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A2>::value, int> = 0,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A3>::value, int> = 0>
void div(const A1 &m1, const A2 &m2, A3 &out)
{
    out.resize(m1.sizes());
    std::transform(m1.begin(),
                   m1.end(),
                   m2.begin(),
                   out.begin(),
                   [](typename A1::value_type v1, typename A2::value_type v2) ->
                   typename A3::value_type { return v1 / v2; });
}
} // namespace iloj::misc::detail

template<typename A1,
         typename A2,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A1>::value, int> = 0,
         std::enable_if_t<iloj::misc::Array::has_dim_iterator<A2>::value, int> = 0>
auto div(const A1 &m1, const A2 &m2) -> typename A1::template promoted_type<typename A2::value_type>
{
    typename A1::template promoted_type<typename A2::value_type> out;
    iloj::misc::detail::div(m1, m2, out);
    return out;
}
