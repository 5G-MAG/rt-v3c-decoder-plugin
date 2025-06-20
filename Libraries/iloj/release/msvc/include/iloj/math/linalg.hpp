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

namespace iloj::math
{
namespace detail
{
template<typename T>
void matprod(shallow::Matrix<T> A, char mA, shallow::Matrix<T> B, char mB, shallow::Matrix<T> C)
{
    if (mA == 'N')
    {
        if (mB == 'N')
        {
            for (auto i = 0ULL; i < C.m(); i++)
            {
                for (auto j = 0ULL; j < C.n(); j++)
                {
                    C(i, j) = std::inner_product(A.row_begin(i), A.row_end(i), B.col_begin(j), T(0));
                }
            }
        }
        else if (mB == 'T')
        {
            for (auto i = 0ULL; i < C.m(); i++)
            {
                for (auto j = 0ULL; j < C.n(); j++)
                {
                    C(i, j) = std::inner_product(A.row_begin(i), A.row_end(i), B.row_begin(j), T(0));
                }
            }
        }
        else
        {
            for (auto i = 0ULL; i < C.m(); i++)
            {
                for (auto j = 0ULL; j < C.n(); j++)
                {
                    C(i, j) = std::inner_product(
                        A.row_begin(i),
                        A.row_end(i),
                        B.row_begin(j),
                        T(0),
                        [](const T &v1, const T &v2) { return (v1 + v2); },
                        [](const T &v1, const T &v2) { return (v1 * conjugate(v2)); });
                }
            }
        }
    }
    else if (mA == 'T')
    {
        if (mB == 'N')
        {
            for (auto i = 0ULL; i < C.m(); i++)
            {
                for (auto j = 0ULL; j < C.n(); j++)
                {
                    C(i, j) = std::inner_product(A.col_begin(i), A.col_end(i), B.col_begin(j), T(0));
                }
            }
        }
        else if (mB == 'T')
        {
            for (auto i = 0ULL; i < C.m(); i++)
            {
                for (auto j = 0ULL; j < C.n(); j++)
                {
                    C(i, j) = std::inner_product(A.col_begin(i), A.col_end(i), B.row_begin(j), T(0));
                }
            }
        }
        else
        {
            for (auto i = 0ULL; i < C.m(); i++)
            {
                for (auto j = 0ULL; j < C.n(); j++)
                {
                    C(i, j) = std::inner_product(
                        A.col_begin(i),
                        A.col_end(i),
                        B.row_begin(j),
                        T(0),
                        [](const T &v1, const T &v2) { return (v1 + v2); },
                        [](const T &v1, const T &v2) { return (v1 * conjugate(v2)); });
                }
            }
        }
    }
    else
    {
        if (mB == 'N')
        {
            for (auto i = 0ULL; i < C.m(); i++)
            {
                for (auto j = 0ULL; j < C.n(); j++)
                {
                    C(i, j) = std::inner_product(
                        A.col_begin(i),
                        A.col_end(i),
                        B.col_begin(j),
                        T(0),
                        [](const T &v1, const T &v2) { return (v1 + v2); },
                        [](const T &v1, const T &v2) { return (conjugate(v1) * v2); });
                }
            }
        }
        else if (mB == 'T')
        {
            for (auto i = 0ULL; i < C.m(); i++)
            {
                for (auto j = 0ULL; j < C.n(); j++)
                {
                    C(i, j) = std::inner_product(
                        A.col_begin(i),
                        A.col_end(i),
                        B.row_begin(j),
                        T(0),
                        [](const T &v1, const T &v2) { return (v1 + v2); },
                        [](const T &v1, const T &v2) { return (conjugate(v1) * v2); });
                }
            }
        }
        else
        {
            for (auto i = 0ULL; i < C.m(); i++)
            {
                for (auto j = 0ULL; j < C.n(); j++)
                {
                    C(i, j) = std::inner_product(
                        A.col_begin(i),
                        A.col_end(i),
                        B.row_begin(j),
                        T(0),
                        [](const T &v1, const T &v2) { return (v1 + v2); },
                        [](const T &v1, const T &v2) { return (conjugate(v1) * conjugate(v2)); });
                }
            }
        }
    }
}
} // namespace detail

template<typename MAT1, typename MAT2, typename MAT3>
auto matprod(const MAT1 &A, char mA, const MAT2 &B, char mB, MAT3 &C) -> MAT3 &
{
    using T = typename MAT1::value_type;

    C.resize((mA == 'N') ? A.m() : A.n(), (mB == 'N') ? B.n() : B.m());
    detail::matprod(shallow::Matrix<T>(A), mA, shallow::Matrix<T>(B), mB, shallow::Matrix<T>(C));
    return C;
}

template<typename MAT>
auto matprod(const MAT &A, char mA, const MAT &B, char mB) -> MAT
{
    MAT C;
    return matprod(A, mA, B, mB, C);
}

namespace detail
{
template<typename T>
void square(shallow::Matrix<T> A, shallow::Matrix<T> out)
{
    matprod(A, 'N', A, 'T', out);
}
} // namespace detail

template<typename MAT1, typename MAT2>
void square(const MAT1 &A, MAT2 &out)
{
    using T = typename MAT1::value_type;

    out.resize(A.m(), A.m());
    detail::square(shallow::Matrix<T>(A), shallow::Matrix<T>(out));
}

template<typename MAT>
auto square(const MAT &A) -> decltype(square_type(A))
{
    decltype(square_type(MAT())) out;
    square(A, out);
    return out;
}

namespace detail
{
template<typename T>
void transquare(shallow::Matrix<T> A, shallow::Matrix<T> out)
{
    detail::matprod(A, 'T', A, 'N', out);
}
} // namespace detail

template<typename MAT1, typename MAT2>
void transquare(const MAT1 &A, MAT2 &out)
{
    using T = typename MAT1::value_type;

    out.resize(A.n(), A.n());
    detail::transquare(shallow::Matrix<T>(A), shallow::Matrix<T>(out));
}

template<typename MAT>
auto transquare(const MAT &A) -> decltype(transquare_type(A))
{
    decltype(transquare_type(MAT())) out;
    transquare(A, out);
    return out;
}

namespace detail
{
template<typename T>
auto PLU(shallow::Matrix<T> A, shallow::Matrix<T> LU, std::vector<int> &P) -> int
{
    int nb_permutations = 0;
    auto n = A.m();

    P.resize(A.m());
    std::iota(P.begin(), P.end(), 0);

    std::copy(A.begin(), A.end(), LU.begin());

    for (auto k = 0ULL; k < n; k++)
    {
        auto iter =
            std::max_element(LU.col_begin(k) + k, LU.col_end(k), [](T x, T y) { return (std::abs(x) < std::abs(y)); });
        T pivot = *iter;

        if (std::abs(std::numeric_limits<T>::epsilon()) < std::abs(pivot))
        {
            auto p = static_cast<Matrix::size_type>(iter - LU.col_begin(k));

            if (p != k)
            {
                std::swap(P[k], P[p]);
                std::swap_ranges(LU.row_begin(k), LU.row_end(k), LU.row_begin(p));
                nb_permutations++;
            }

            for (auto i = k + 1; i < n; i++)
            {
                T factor = (LU(i, k) /= pivot);
                std::transform(LU.row_begin(i) + (k + 1),
                               LU.row_end(i),
                               LU.row_begin(k) + (k + 1),
                               LU.row_begin(i) + (k + 1),
                               [factor](T v1, T v2) { return v1 - factor * v2; });
            }
        }
        else
        {
            return -1;
        }
    }

    return nb_permutations;
}
} // namespace detail

template<typename MAT1, typename MAT2>
auto PLU(const MAT1 &A, MAT2 &LU, std::vector<int> &P) -> int
{
    using T = typename MAT1::value_type;

    LU.resize(A.m(), A.n());
    return detail::PLU(shallow::Matrix<T>(A), shallow::Matrix<T>(LU), P);
}

template<typename MAT1, typename MAT2>
auto PLU(const MAT1 &A, MAT2 &L, MAT2 &U, MAT2 &P) -> int
{
    using T1 = typename MAT2::value_type;
    using T2 = typename MAT2::value_type;

    heap::Matrix<T1> LU;
    std::vector<int> p;

    int ret = PLU(A, LU, p);

    L.resize(A.m(), A.n());
    U.resize(A.m(), A.n());
    P.resize(A.m(), A.n());

    std::fill(P.begin(), P.end(), T2(0));

    for (auto i = 0ULL; i < A.m(); i++)
    {
        P(i, p[i]) = 1;

        for (auto j = 0ULL; j < A.m(); j++)
        {
            if (j < i)
            {
                L(i, j) = LU(i, j);
                U(i, j) = T2(0);
            }
            else if (j == i)
            {
                L(i, i) = T2(1);
                U(i, j) = LU(i, j);
            }
            else
            {
                L(i, j) = T2(0);
                U(i, j) = LU(i, j);
            }
        }
    }

    return ret;
}

namespace detail
{
template<typename T>
auto chol(shallow::Matrix<T> A, shallow::Matrix<T> out) -> int
{
    auto n = A.m();
    T x;

    // First column
    out(0, 0) = x = sqrt(A(0, 0));

    for (auto i = 1ULL; i < n; i++)
    {
        out(i, 0) = A(i, 0) / x;
    }

    // Remaining lower part
    for (auto j = 1ULL; j < n; j++)
    {
        for (auto i = 0ULL; i < j; i++)
        {
            out(i, j) = 0;
        }

        out(j, j) = x = std::sqrt(A(j, j) - dot_product(out.row_begin(j), out.row_begin(j) + j, out.row_begin(j)));

        for (auto i = (j + 1); i < n; i++)
        {
            out(i, j) = (A(i, j) - dot_product(out.row_begin(i), out.row_begin(i) + j, out.row_begin(j))) / x;
        }
    }

    return 0;
}
} // namespace detail

template<typename MAT1, typename MAT2>
auto chol(const MAT1 &A, MAT2 &out) -> int
{
    using T = typename MAT1::value_type;

    out.resize(A.m(), A.m());
    return detail::chol(shallow::Matrix<T>(A), shallow::Matrix<T>(out));
}

template<typename MAT>
auto chol(const MAT &A, int *info) -> MAT
{
    MAT out;

    int information = chol(A, out);
    if (info)
    {
        *info = information;
    }

    return out;
}

namespace detail
{
template<typename T>
auto det(shallow::Matrix<T> A, int *info) -> T
{
    heap::Matrix<T> LU;
    std::vector<int> P;
    int n = iloj::math::PLU(A, LU, P);
    T d = std::accumulate(LU.diag_begin(), LU.diag_end(), T(1), [](T v1, T v2) { return (v1 * v2); });

    if (n % 2)
    {
        d = -d;
    }

    if (info)
    {
        (*info) = 0;
    }

    return d;
}
} // namespace detail

template<typename MAT>
auto det(const MAT &A, int *info) -> typename MAT::value_type
{
    int information = 0;
    typename MAT::value_type out = 0;

    if (A.isPositive())
    {
        MAT L = chol(A, &information);

        if (!information)
        {
            out = 1;
            std::for_each(L.diag_begin(), L.diag_end(), [&out](typename MAT::value_type t) { out *= t; });
            out = sqr(out);
        }
    }
    else
    {
        using T = typename MAT::value_type;

        out = detail::det(shallow::Matrix<T>(A), &information);
    }

    if (info)
    {
        (*info) = information;
    }

    return out;
}

namespace detail
{
template<typename T>
auto mldivide(shallow::Matrix<T> A, shallow::Matrix<T> B, shallow::Matrix<T> out) -> int
{
    // PLU decomposition
    heap::Matrix<T> LU;
    std::vector<int> P;

    iloj::math::PLU(A, LU, P);

    // Solve Y = inv(L) * B
    heap::Matrix<T> Y({B.m(), B.n()});

    for (auto i = 0ULL; i < Y.m(); i++)
    {
        for (auto j = 0ULL; j < Y.n(); j++)
        {
            Y(i, j) = B(P[i], j);

            if (0 < i)
            {
                Y(i, j) -= std::inner_product(LU.row_begin(i), LU.row_begin(i) + i, Y.col_begin(j), T(0));
            }
        }
    }

    // Solve out = inv(U) * Y
    auto m = out.m();

    for (auto i = 0ULL; i < Y.m(); i++)
    {
        for (auto j = 0ULL; j < out.n(); j++)
        {
            auto k = m - i - 1;

            out(k, j) = Y(k, j) / LU(k, k);

            if (0 < i)
            {
                out(k, j) -=
                    std::inner_product(LU.row_begin(k) + m - i, LU.row_end(k), out.col_begin(j) + m - i, T(0)) /
                    LU(k, k);
            }
        }
    }

    return 0;
}
} // namespace detail

template<typename MAT1, typename MAT2, typename MAT3>
auto mldivide(const MAT1 &A, const MAT2 &B, MAT3 &out) -> int
{
    using T = typename MAT1::value_type;

    out.resize(B.m(), B.n());
    return detail::mldivide(shallow::Matrix<T>(A), shallow::Matrix<T>(B), shallow::Matrix<T>(out));
}

template<typename MAT1, typename MAT2>
auto mldivide(const MAT1 &A, const MAT2 &B, int *info) -> MAT2
{
    MAT2 out;

    int information = mldivide(A, B, out);
    if (info)
    {
        *info = information;
    }

    return out;
}

namespace detail
{
template<typename T>
auto mrdivide(shallow::Matrix<T> A, shallow::Matrix<T> B, shallow::Matrix<T> out) -> int
{
    // PLU decomposition
    heap::Matrix<T> LU;
    std::vector<int> P;

    iloj::math::PLU(B, LU, P);

    // Solve Y = A * inv(U)
    heap::Matrix<T> Y({A.m(), A.n()});

    for (auto i = 0ULL; i < Y.m(); i++)
    {
        for (auto j = 0ULL; j < Y.n(); j++)
        {
            Y(i, j) = A(i, j) / LU(j, j);

            if (0 < j)
            {
                Y(i, j) -= std::inner_product(Y.row_begin(i), Y.row_begin(i) + j, LU.col_begin(j), T(0)) / LU(j, j);
            }
        }
    }

    // Solve out = Y * inv(L)
    auto n = out.n();

    for (auto i = 0ULL; i < Y.m(); i++)
    {
        for (auto j = 0ULL; j < n; j++)
        {
            auto k = n - j - 1;

            if (0 < j)
            {
                Y(i, k) -= std::inner_product(Y.row_begin(i) + n - j, Y.row_end(i), LU.col_begin(k) + n - j, T(0));
            }

            out(i, P[k]) = Y(i, k);
        }
    }

    return 0;
}
} // namespace detail

template<typename MAT1, typename MAT2, typename MAT3>
auto mrdivide(const MAT1 &A, const MAT2 &B, MAT3 &out) -> int
{
    using T = typename MAT1::value_type;

    out.resize(A.m(), A.n());
    return detail::mrdivide(shallow::Matrix<T>(A), shallow::Matrix<T>(B), shallow::Matrix<T>(out));
}

template<typename MAT1, typename MAT2>
auto mrdivide(const MAT1 &A, const MAT2 &B, int *info) -> MAT1
{
    MAT1 out;

    int information = mrdivide(A, B, out);
    if (info)
    {
        *info = information;
    }

    return out;
}

namespace detail
{
template<typename T>
auto inv(shallow::Matrix<T> A, shallow::Matrix<T> out) -> int
{
    auto I = heap::Matrix<T>::eye({A.m(), A.m()});
    return iloj::math::detail::mldivide(std::move(A), shallow::Matrix<T>(I), std::move(out));
}
} // namespace detail

template<typename MAT1, typename MAT2>
auto inv(const MAT1 &A, MAT2 &out) -> int
{
    using T = typename MAT1::value_type;

    out.resize(A.m(), A.n());
    return detail::inv(shallow::Matrix<T>(A), shallow::Matrix<T>(out));
}

template<typename MAT>
auto inv(const MAT &A, int *info) -> MAT
{
    MAT out;
    int information = inv(A, out);

    if (info)
    {
        *info = information;
    }

    return out;
}

namespace detail
{
/*
template<typename T>
void hess(shallow::Matrix<T> A, shallow::Matrix<T> H, shallow::Matrix<T> *U)
{
    auto n = A.m();
    using norm_type = decltype(std::abs(typename T::value_type(0)));

    for(std::size_t k = 0; k < (n - 1); k++)
    {
        // Householder reflector
        heap::Vector<T> v({n - (k + 1)});
        std::copy(A.col_begin(k) + (k + 1), A.col_end(k));

        auto alpha = norm(v);
        v[0] -= alpha;

        auto beta = static_cast<norm_type>(1) / norm(v);
        v *= beta;

        matprod(v, 'H', )


    }

for k=1:(n-2)

v = A((k+1):n, k);
alpha = norm(v);
v(1) = v(1) - alpha;
beta = 1 / norm(v);
v = v * beta;

A((k+1):n,k:n) = A((k+1):n,k:n) - 2 * v * (v' * A((k+1):n,k:n));
A(1:n,(k+1):n) = A(1:n,(k+1):n) - 2 * (A(1:n,(k+1):n) * v) * v';

V((k+1):n, k) = v;

endfor



U = eye(n);
V = zeros(n);



for k=(n-2):-1:1

v = V((k+1):n, k);
U((k+1):n, (k+1):n) = U((k+1):n, (k+1):n) - 2 * v * (v' * U((k+1):n, (k+1):n));

endfor



    // auto I = heap::Matrix<T>::eye({A.m(), A.m()});
    // return iloj::math::detail::mldivide(std::move(A), shallow::Matrix<T>(I), std::move(out));
}
*/
} // namespace detail

// template<typename MAT1, typename MAT2>
// void hess(const MAT1 &A, MAT2 &out, MAT2 &U);
// template<typename MAT1, typename MAT2>
// void hess(const MAT1 &A, MAT2 &out);
// template<typename MAT>
// auto hess(const MAT &A) -> MAT;

} // namespace iloj::math

template<typename T, typename U>
auto operator*(const iloj::math::heap::Matrix<T> &A, const iloj::math::heap::Vector<U> &B)
    -> iloj::math::heap::Vector<decltype(T(0) * U(0))>
{
    iloj::math::heap::Vector<decltype(T(0) * U(0))> out;
    return iloj::math::matprod(A, 'N', B, 'N', out);
}

template<typename T, typename U, iloj::math::Matrix::size_type M>
auto operator*(const iloj::math::heap::Matrix<T> &A, const iloj::math::stack::Vector<U, M> &B)
    -> iloj::math::heap::Vector<decltype(T(0) * U(0))>
{
    iloj::math::heap::Vector<decltype(T(0) * U(0))> out;
    return iloj::math::matprod(A, 'N', B, 'N', out);
}

template<typename T, typename U>
auto operator*(const iloj::math::heap::Matrix<T> &A, const iloj::math::heap::Matrix<U> &B)
    -> iloj::math::heap::Matrix<decltype(T(0) * U(0))>
{
    iloj::math::heap::Matrix<decltype(T(0) * U(0))> out;
    return iloj::math::matprod(A, 'N', B, 'N', out);
}

template<typename T, typename U, iloj::math::Matrix::size_type M, iloj::math::Matrix::size_type N>
auto operator*(const iloj::math::heap::Matrix<T> &A, const iloj::math::stack::Matrix<U, M, N> &B)
    -> iloj::math::heap::Matrix<decltype(T(0) * U(0))>
{
    iloj::math::heap::Matrix<decltype(T(0) * U(0))> out;
    return iloj::math::matprod(A, 'N', B, 'N', out);
}

template<typename T, typename U, iloj::math::Matrix::size_type M, iloj::math::Matrix::size_type N>
auto operator*(const iloj::math::stack::Matrix<T, M, N> &A, const iloj::math::stack::Vector<U, N> &B)
    -> iloj::math::stack::Vector<decltype(T(0) * U(0)), M>
{
    iloj::math::stack::Vector<decltype(T(0) * U(0)), M> out;
    return iloj::math::matprod(A, 'N', B, 'N', out);
}

template<typename T, typename U, iloj::math::Matrix::size_type M, iloj::math::Matrix::size_type N>
auto operator*(const iloj::math::stack::Matrix<T, M, N> &A, const iloj::math::heap::Vector<U> &B)
    -> iloj::math::heap::Vector<decltype(T(0) * U(0))>
{
    iloj::math::heap::Vector<decltype(T(0) * U(0))> out;
    return iloj::math::matprod(A, 'N', B, 'N', out);
}

template<typename T, typename U, iloj::math::Matrix::size_type M, iloj::math::Matrix::size_type N>
auto operator*(const iloj::math::stack::Matrix<T, M, N> &A, const iloj::math::heap::Matrix<U> &B)
    -> iloj::math::heap::Matrix<decltype(T(0) * U(0))>
{
    iloj::math::heap::Matrix<decltype(T(0) * U(0))> out;
    return iloj::math::matprod(A, 'N', B, 'N', out);
}

template<typename T,
         typename U,
         iloj::math::Matrix::size_type M,
         iloj::math::Matrix::size_type N,
         iloj::math::Matrix::size_type O>
auto operator*(const iloj::math::stack::Matrix<T, M, N> &A, const iloj::math::stack::Matrix<U, N, O> &B)
    -> iloj::math::stack::Matrix<decltype(T(0) * U(0)), M, O>
{
    iloj::math::stack::Matrix<decltype(T(0) * U(0)), M, O> out;
    return iloj::math::matprod(A, 'N', B, 'N', out);
}
