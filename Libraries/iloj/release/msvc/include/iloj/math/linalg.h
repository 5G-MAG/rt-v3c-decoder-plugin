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

#include "matrix.h"
#include "vector.h"

namespace iloj::math
{
//! \brief Matrix product.
template<typename MAT1, typename MAT2, typename MAT3>
auto matprod(const MAT1 &A, char mA, const MAT2 &B, char mB, MAT3 &C) -> MAT3 &;
template<typename MAT>
auto matprod(const MAT &A, char mA, const MAT &B, char mB) -> MAT;

//! \brief Computes and returns A * A'.
template<typename T, Matrix::size_type M>
auto square_type(stack::Vector<T, M>) -> stack::Matrix<T, M, M>;
template<typename T, Matrix::size_type M, Matrix::size_type N>
auto square_type(stack::Matrix<T, M, N>) -> stack::Matrix<T, M, M>;
template<typename T>
auto square_type(heap::Vector<T>) -> heap::Matrix<T>;
template<typename T>
auto square_type(heap::Matrix<T>) -> heap::Matrix<T>;

template<typename MAT1, typename MAT2>
void square(const MAT1 &A, MAT2 &out);
template<typename MAT>
auto square(const MAT &A) -> decltype(square_type(A));

//! \brief Computes and returns A' * A.
template<typename T, Matrix::size_type M, Matrix::size_type N>
auto transquare_type(stack::Matrix<T, M, N>) -> stack::Matrix<T, N, N>;
template<typename T>
auto transquare_type(heap::Matrix<T>) -> heap::Matrix<T>;

template<typename MAT1, typename MAT2>
void transquare(const MAT1 &A, MAT2 &out);
template<typename MAT>
auto transquare(const MAT &A) -> decltype(transquare_type(A));

//! \brief Computes the PLU factorization of the matrix A.
//! \param[in] A Matrix to decompose.
//! \param[out] LU LU factors (packed version).
//! \param[out] P Pivots indices.
//! \return Error code. EC = 0: Successfull exit.
template<typename MAT1, typename MAT2>
auto PLU(const MAT1 &A, MAT2 &LU, std::vector<int> &P) -> int;
template<typename MAT1, typename MAT2>
auto PLU(const MAT1 &A, MAT2 &L, MAT2 &U, MAT2 &P) -> int;

//! \brief Computes the Cholesky factorization of a real symmetric or complex
//! hermitian positive definite matrix A. \param[in] A Real symmetric or complex
//! hermitian positive definite matrix to decompose. \param[out] info Pointer to
//! an error code. EC = 0: Successfull exit. EC < 0: Bad parameter. EC > 0: The
//! given matrix is not positive definite. \return New matrix containing the
//! Cholesky factorization of A.
template<typename MAT1, typename MAT2>
auto chol(const MAT1 &A, MAT2 &out) -> int;
template<typename MAT>
auto chol(const MAT &A, int *info = nullptr) -> MAT;

//! \brief Computes and returns the determinant of a square matrix A.
template<typename MAT>
auto det(const MAT &A, int *info = nullptr) -> typename MAT::value_type;

//! \brief Solves the system AX = B (ie X = inv(A) * B) and return the solution
//! matrix. \param[in] A Square matrix A. \param[in] B Rectangular matrix B.
//! \param[out] info Optional pointer to an error code (default equal to
//! nullptr). EC = 0: Successfull exit. EC < 0: Bad parameter. \return New
//! matrix containing the solution of the system AX = B.
template<typename MAT1, typename MAT2, typename MAT3>
auto mldivide(const MAT1 &A, const MAT2 &B, MAT3 &out) -> int;
template<typename MAT1, typename MAT2>
auto mldivide(const MAT1 &A, const MAT2 &B, int *info = nullptr) -> MAT2;

//! \brief Solves the system XB = A (ie X = A * inv(B)) and returns the solution
//! matrix. \param[in] A Rectangular matrix A. \param[in] B Square matrix B.
//! \param[out] info Optional pointer to an error code (default equal to
//! nullptr). EC = 0: Successfull exit. EC < 0: Bad parameter. \return New
//! matrix containing the solution of the system XB = A.
template<typename MAT1, typename MAT2, typename MAT3>
auto mrdivide(const MAT1 &A, const MAT2 &B, MAT3 &out) -> int;
template<typename MAT1, typename MAT2>
auto mrdivide(const MAT1 &A, const MAT2 &B, int *info = nullptr) -> MAT1;

//! \brief Computes and return the inverse of A.
//! \param[in] A Square matrix A.
//! \param[out] info Optional pointer to an error code (default equal to
//! nullptr). EC = 0: Successfull exit. EC < 0: Bad parameter. EC > 0: The
//! matrix is singular. \return New matrix containing the inverse of A.
template<typename MAT1, typename MAT2>
auto inv(const MAT1 &A, MAT2 &out) -> int;
template<typename MAT>
auto inv(const MAT &A, int *info = nullptr) -> MAT;

//! \brief Computes and returns the Hessenberg reduction of a square matrix A and optionally, the associated
//! transformation matrix.
template<typename MAT1, typename MAT2>
void hess(const MAT1 &A, MAT2 &out, MAT2 &U);
template<typename MAT1, typename MAT2>
void hess(const MAT1 &A, MAT2 &out);
template<typename MAT>
auto hess(const MAT &A) -> MAT;

} // namespace iloj::math

//! \brief Matrix product operator
template<typename T, typename U>
auto operator*(const iloj::math::heap::Matrix<T> &A, const iloj::math::heap::Vector<U> &B)
    -> iloj::math::heap::Vector<decltype(T(0) * U(0))>;
template<typename T, typename U, iloj::math::Matrix::size_type M>
auto operator*(const iloj::math::heap::Matrix<T> &A, const iloj::math::stack::Vector<U, M> &B)
    -> iloj::math::heap::Vector<decltype(T(0) * U(0))>;
template<typename T, typename U>
auto operator*(const iloj::math::heap::Matrix<T> &A, const iloj::math::heap::Matrix<U> &B)
    -> iloj::math::heap::Matrix<decltype(T(0) * U(0))>;
template<typename T, typename U, iloj::math::Matrix::size_type M, iloj::math::Matrix::size_type N>
auto operator*(const iloj::math::heap::Matrix<T> &A, const iloj::math::stack::Matrix<U, M, N> &B)
    -> iloj::math::heap::Matrix<decltype(T(0) * U(0))>;

template<typename T, typename U, iloj::math::Matrix::size_type M, iloj::math::Matrix::size_type N>
auto operator*(const iloj::math::stack::Matrix<T, M, N> &A, const iloj::math::stack::Vector<U, N> &B)
    -> iloj::math::stack::Vector<decltype(T(0) * U(0)), M>;
template<typename T, typename U, iloj::math::Matrix::size_type M, iloj::math::Matrix::size_type N>
auto operator*(const iloj::math::stack::Matrix<T, M, N> &A, const iloj::math::heap::Vector<U> &B)
    -> iloj::math::heap::Vector<decltype(T(0) * U(0))>;
template<typename T, typename U, iloj::math::Matrix::size_type M, iloj::math::Matrix::size_type N>
auto operator*(const iloj::math::stack::Matrix<T, M, N> &A, const iloj::math::heap::Matrix<U> &B)
    -> iloj::math::heap::Matrix<decltype(T(0) * U(0))>;
template<typename T,
         typename U,
         iloj::math::Matrix::size_type M,
         iloj::math::Matrix::size_type N,
         iloj::math::Matrix::size_type O>
auto operator*(const iloj::math::stack::Matrix<T, M, N> &A, const iloj::math::stack::Matrix<U, N, O> &B)
    -> iloj::math::stack::Matrix<decltype(T(0) * U(0)), M, O>;

#include "linalg.hpp"
