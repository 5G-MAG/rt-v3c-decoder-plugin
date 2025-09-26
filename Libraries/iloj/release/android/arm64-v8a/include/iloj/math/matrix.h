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

#include <iloj/misc/array.h>

namespace iloj::math
{
namespace Matrix
{
enum Property
{
    None,
    Symmetric,
    Hermitian,
    Positive,
    Lower,
    Upper
};

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
    [[nodiscard]] auto n() const -> size_type { return A::size(1); }
    //! \brief Returns the number of rows of the matrix.
    [[nodiscard]] auto height() const -> size_type { return A::size(0); }
    //! \brief Returns the number of columns of the matrix.
    [[nodiscard]] auto width() const -> size_type { return A::size(1); }
    //! \brief Returns the leading dimension of the matrix.
    [[nodiscard]] auto lda() const -> size_type { return A::size(1); }
    //! \brief Overloaded resize operator.
    using A::resize;
    void resize(size_type a, size_type b) { A::resize({a, b}); }
    //! \brief Returns an iterator to the first element of the ith row.
    [[nodiscard]] auto row_begin(size_type i) const -> const_row_iterator { return A::template dim_begin<1>(i); }
    auto row_begin(size_type i) -> row_iterator { return A::template dim_begin<1>(i); }
    //! \brief Returns a const iterator to the first element of the ith row.
    [[nodiscard]] auto crow_begin(size_type i) const -> const_row_iterator { return A::template cdim_begin<1>(i); }
    //! \brief Returns an iterator to the first element after the end of the ith
    //! row.
    [[nodiscard]] auto row_end(size_type i) const -> const_row_iterator { return A::template dim_end<1>(i); }
    auto row_end(size_type i) -> row_iterator { return A::template dim_end<1>(i); }
    //! \brief Returns a const iterator to the first element after the end of the
    //! ith row.
    [[nodiscard]] auto crow_end(size_type i) const -> const_row_iterator { return A::template cdim_end<1>(i); }
    //! \brief Returns an iterator to the first element of the jth column.
    [[nodiscard]] auto col_begin(size_type j) const -> const_column_iterator { return A::template dim_begin<0>(j); }
    auto col_begin(size_type j) -> column_iterator { return A::template dim_begin<0>(j); }
    //! \brief Returns a const iterator to the first element of the jth column.
    [[nodiscard]] auto ccol_begin(size_type j) const -> const_column_iterator { return A::template cdim_begin<0>(j); }
    //! \brief Returns an iterator to the first element after the end of the jth
    //! column.
    [[nodiscard]] auto col_end(size_type j) const -> const_column_iterator { return A::template dim_end<0>(j); }
    auto col_end(size_type j) -> column_iterator { return A::template dim_end<0>(j); }
    //! \brief Returns a const iterator to the first element after the end of the
    //! jth column.
    [[nodiscard]] auto ccol_end(size_type j) const -> const_column_iterator { return A::template cdim_end<0>(j); }
    //! \brief Returns true if the matrix is a row.
    [[nodiscard]] auto isRow() const -> bool { return (m() == 1); }
    //! \brief Returns true if the matrix is a column.
    [[nodiscard]] auto isColumn() const -> bool { return (n() == 1); }
    //! \brief Returns true if the matrix is symmetric.
    [[nodiscard]] auto isSymmetric() const -> bool
    {
        return (A::getProperty() == Matrix::Property::Symmetric) || (A::getProperty() == Matrix::Property::Positive);
    }
    //! \brief Returns true if the matrix is hermitian.
    [[nodiscard]] auto isHermitian() const -> bool { return (A::getProperty() == Matrix::Property::Hermitian); }
    //! \brief Returns true if the matrix is positive.
    [[nodiscard]] auto isPositive() const -> bool { return (A::getProperty() == Matrix::Property::Positive); }
    //! \brief Returns true if the matrix is lower.
    [[nodiscard]] auto isLower() const -> bool { return (A::getProperty() == Matrix::Property::Lower); }
    //! \brief Returns true if the matrix is upper.
    [[nodiscard]] auto isUpper() const -> bool { return (A::getProperty() == Matrix::Property::Upper); }
    //! \brief Returns true if the matrix is triangular.
    [[nodiscard]] auto isTriangular() const -> bool
    {
        return (A::getProperty() == Matrix::Property::Lower) || (A::getProperty() == Matrix::Property::Upper);
    }
    static auto diag(const std::vector<typename A::value_type> &v) -> Interface
    {
        Interface out;

        out.resize(v.size(), v.size());
        std::fill(out.begin(), out.end(), 0);
        std::copy(v.begin(), v.end(), out.diag_begin());

        return out;
    }
};

using size_type = iloj::misc::Array::size_type;
} // namespace Matrix

namespace stack
{
template<typename T, Matrix::size_type M, Matrix::size_type N>
using Matrix = Matrix::Interface<iloj::misc::stack::Array<T, M, N>>;

template<typename T>
using Mat2x2 = Matrix<T, 2, 2>;
template<typename T>
using Mat3x3 = Matrix<T, 3, 3>;
template<typename T>
using Mat4x4 = Matrix<T, 4, 4>;

} // namespace stack

namespace heap
{
template<typename T>
using Matrix = Matrix::Interface<iloj::misc::heap::Array<2, T>>;
}

namespace shallow
{
template<typename T>
using Matrix = Matrix::Interface<iloj::misc::shallow::Array<2, T>>;
}

// Additional definitions
using Mat2x2i = stack::Mat2x2<int>;
using Mat2x2f = stack::Mat2x2<float>;
using Mat2x2d = stack::Mat2x2<double>;
using Mat3x3i = stack::Mat3x3<int>;
using Mat3x3f = stack::Mat3x3<float>;
using Mat3x3d = stack::Mat3x3<double>;
using Mat4x4i = stack::Mat4x4<int>;
using Mat4x4f = stack::Mat4x4<float>;
using Mat4x4d = stack::Mat4x4<double>;
template<typename T>
using Mat = heap::Matrix<T>;

//! \brief Returns the type of the transpose of the matrix given as input.
template<typename T, Matrix::size_type M, Matrix::size_type N>
auto transpose_type(stack::Matrix<T, M, N>) -> stack::Matrix<T, N, M>;
template<typename T>
auto transpose_type(heap::Matrix<T>) -> heap::Matrix<T>;

//! \brief Returns the transpose of the matrix given as input.
template<typename Mat1, typename Mat2>
auto transpose(const Mat1 &in, Mat2 &out) -> Mat2 &
{
    out.resize({in.n(), in.m()});

    if (in.isRow() || in.isColumn() || in.isSymmetric())
    {
        std::copy(in.begin(), in.end(), out.begin());
    }
    else
    {
        for (auto i = 0ULL; i < out.m(); i++)
        {
            std::copy(in.col_begin(i), in.col_end(i), out.row_begin(i));
        }
    }

    return out;
}

template<typename Mat>
auto transpose(const Mat &m) -> decltype(transpose_type(m))
{
    decltype(transpose_type(Mat())) out;
    return transpose(m, out);
}

//! \brief Computes and returns the adjoint of the matrix a.
template<typename Mat1, typename Mat2>
auto adjoint(const Mat1 &in, Mat2 &out) -> Mat2 &
{
    out.resize({in.n(), in.m()});

    if (in.isRow() || in.isColumn())
    {
        std::transform(
            in.begin(), in.end(), out.begin(), [](const typename Mat1::value_type &v) { return conjugate(v); });
    }
    else if (in.isHermitian())
    {
        std::copy(in.begin(), in.end(), out.begin());
    }
    else
    {
        for (auto i = 0ULL; i < out.m(); i++)
        {
            std::transform(in.col_begin(i), in.col_end(i), out.row_begin(i), [](const typename Mat1::value_type &v) {
                return conjugate(v);
            });
        }
    }

    return out;
}

template<typename Mat>
auto adjoint(const Mat &m) -> decltype(transpose_type(m))
{
    decltype(transpose_type(Mat())) out;
    return adjoint(m, out);
}

//! \brief Symmetrizes the matrix A by filling its lower (mode == 'L') or upper
//! (mode == 'U') part.
template<typename Mat>
void symmetrize(Mat &A, char mode = 'L')
{
    if (mode == 'L')
    {
        for (auto i = 1ULL; i < A.m(); i++)
        {
            auto ptr1 = A.row_begin(i);
            auto ptr2 = A.col_begin(i);

            std::copy(ptr2, ptr2 + i, ptr1);
        }
    }
    else
    {
        for (auto i = 1ULL; i < A.m(); i++)
        {
            auto ptr1 = A.row_begin(i);
            auto ptr2 = A.col_begin(i);

            std::copy(ptr1, ptr1 + i, ptr2);
        }
    }
}

//! \brief Hermitianizes the lower (mode == 'L') or upper (mode == 'U') part of
//! the matrix A.
template<typename Mat>
void hermitianize(Mat &A, char mode = 'L')
{
    if (mode == 'L')
    {
        for (auto i = 1ULL; i < A.m(); i++)
        {
            auto ptr1 = A.row_begin(i);
            auto ptr2 = A.col_begin(i);

            std::transform(ptr2, ptr2 + i, ptr1, [](const typename Mat::value_type &v) { return std::conj(v); });
        }
    }
    else
    {
        for (auto i = 1ULL; i < A.m(); i++)
        {
            auto ptr1 = A.row_begin(i);
            auto ptr2 = A.col_begin(i);

            std::transform(ptr1, ptr1 + i, ptr2, [](const typename Mat::value_type &v) { return std::conj(v); });
        }
    }
}

//! \brief Mirrors the  the matrix A.
template<typename Mat>
void flip(Mat &A, bool vertical, bool horizontal)
{
    auto h = A.m();

    if (vertical)
    {
        for (std::size_t i1 = 0U, i2 = (h - 1); i1 < (h / 2); i1++, i2--)
        {
            std::swap_ranges(A.row_begin(i1), A.row_end(i1), A.row_begin(i2));
        }
    }

    if (horizontal)
    {
        for (std::size_t i = 0U; i < h; i++)
        {
            std::reverse(A.row_begin(i), A.row_end(i));
        }
    }
}

//! \brief Computes and returns the trace of the matrix a.
template<typename Mat>
auto trace(const Mat &a) -> typename Mat::value_type
{
    return std::accumulate(a.diag_begin(), a.diag_end(), typename Mat::value_type(0));
}

//! \brief Constructs a block matrix from the matrices given as input.
template<typename Mat1, typename Mat2>
auto block(std::initializer_list<std::initializer_list<Mat1>> L, Mat2 &out) -> Mat2 &
{
    // Number of rows
    auto m0 = 0ULL;
    for (auto iter = L.begin(); iter != L.end(); iter++)
    {
        m0 += iter->begin()->m();
    }

    // Number of columns
    auto n0 = 0ULL;
    for (auto iter = L.begin()->begin(); iter != L.begin()->end(); iter++)
    {
        n0 += iter->n();
    }

    out.resize({m0, n0});

    // Building
    auto i0 = 0ULL;
    auto j0 = 0ULL;

    for (auto iter1 = L.begin(); iter1 != L.end(); iter1++)
    {
        j0 = 0;

        for (auto iter2 = iter1->begin(); iter2 != iter1->end(); iter2++)
        {
            for (auto i = 0ULL; i < iter2->m(); i++)
            {
                std::copy(iter2->row_begin(i), iter2->row_end(i), out.row_begin(i0 + i) + j0);
            }

            j0 += iter2->n();
        }

        i0 += iter1->begin()->m();
    }

    return out;
}

template<typename Mat>
auto block(std::initializer_list<std::initializer_list<Mat>> L) -> heap::Matrix<typename Mat::value_type>
{
    heap::Matrix<typename Mat::value_type> out;
    block(L, out);
    return out;
}

//! \brief Replicates and tiles matrix a according to the dimension vector dim.
template<typename Mat1, typename Mat2>
auto repmat(const std::array<Matrix::size_type, 2> &dim, const Mat1 &a, Mat2 &out) -> Mat2 &
{
    out.resize({dim[0] * a.m(), dim[1] * a.n()});

    for (auto i = 0ULL, i0 = 0ULL; i < dim[0]; i++, i0 += a.m())
    {
        for (auto j = 0ULL, j0 = 0ULL; j < dim[1]; j++, j0 += a.n())
        {
            for (auto k = 0ULL; k < a.m(); k++)
            {
                std::copy(a.row_begin(k), a.row_end(k), out.row_begin(i0 + k) + j0);
            }
        }
    }

    return out;
}

template<typename Mat>
auto repmat(const std::array<Matrix::size_type, 2> &dim, const Mat &a) -> heap::Matrix<typename Mat::value_type>
{
    heap::Matrix<typename Mat::value_type> out;
    repmat(dim, a, out);
    return out;
}

template<typename Mat>
void sat(const Mat &in, Mat &out)
{
    out.resize(in.sizes());

    std::partial_sum(in.row_begin(0), in.row_end(0), out.row_begin(0));
    std::partial_sum(in.col_begin(0), in.col_end(0), out.col_begin(0));

    for (auto i = 1ULL; i < out.m(); i++)
    {
        for (auto j = 1ULL; j < out.n(); j++)
        {
            out(i, j) = in(i, j) + out(i, j - 1) + out(i - 1, j) - out(i - 1, j - 1);
        }
    }
}

template<typename Mat>
auto sat(const Mat &in) -> Mat
{
    Mat out;
    sat(in, out);
    return out;
}

template<typename Mat>
auto sat(const Mat &sat, std::size_t i0, std::size_t j0, std::size_t i1, std::size_t j1) -> typename Mat::value_type
{
    if ((i0 == 0) && (j0 == 0))
    {
        return sat(i1 - 1, j1 - 1);
    }

    if (i0 == 0)
    {
        return sat(i1 - 1, j1 - 1) - sat(i1 - 1, j0 - 1);
    }

    if (j0 == 0)
    {
        return sat(i1 - 1, j1 - 1) - sat(i0 - 1, j1 - 1);
    }

    return (sat(i1 - 1, j1 - 1) + sat(i0 - 1, j0 - 1)) - (sat(i1 - 1, j0 - 1) + sat(i0 - 1, j1 - 1));
}
} // namespace iloj::math
