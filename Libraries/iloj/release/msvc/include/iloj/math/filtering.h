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

#include "linalg.h"
#include <functional>

namespace iloj::math
{
//! \brief Class implementing a basic Finite Impulse Response (FIR) filter.
class FIR
{
public:
    struct Type
    {
        enum
        {
            LOW = 1,
            HIGH = 2,
            BAND = 4
        };
    };
    using value_type = std::vector<double>::value_type;
    using reference = std::vector<double>::reference;
    using const_reference = std::vector<double>::const_reference;
    using iterator = std::vector<double>::iterator;
    using const_iterator = std::vector<double>::const_iterator;
    using difference_type = std::vector<double>::difference_type;
    using size_type = std::vector<double>::size_type;
    using container_type = FIR;

private:
    int m_iMin{0};
    std::vector<double> m_coefficients;

public:
    FIR() = default;
    //! \brief Overloaded constructor. Builds an uninitialized filter of size n and with a minimal temporal indice equal
    //! to imin.
    explicit FIR(unsigned s, int imin = 0): m_iMin(imin), m_coefficients(s) {}
    //! \brief Overloaded constructor. Builds an array containing the values given as inputs and with a minimal temporal
    //! indice equal to imin.
    FIR(const std::initializer_list<double> &values, int imin = 0): m_iMin(imin), m_coefficients(values) {}
    //! \brief Designs a FIR digital filter (windowing design method). The cut-off frequencies must be between 0 < Wn
    //! < 1.0, with 1.0 corresponding to the sample rate. The filter is real and has linear phase. \param[in] order
    //! Filter order (adapted to be odd). \param[in] type Filter type.
    //! Maybe LOW (lowpass), HIGH (highpass), BAND (bandpass) \param[in] w1 Cut off frequency.
    //! \param[in] w2 Optional second cut off frequency (for bandpass and stopband filters).
    FIR(unsigned order, unsigned type, double w1, double w2 = 0.);
    ~FIR() = default;
    FIR(const FIR &f) = default;
    FIR(FIR &&f) = default;
    auto operator=(const FIR &f) -> FIR & = default;
    auto operator=(FIR &&f) -> FIR & = default;
    auto begin() -> iterator { return m_coefficients.begin(); }
    auto end() -> iterator { return m_coefficients.end(); }
    [[nodiscard]] auto begin() const -> const_iterator { return m_coefficients.begin(); }
    [[nodiscard]] auto end() const -> const_iterator { return m_coefficients.end(); }
    [[nodiscard]] auto cbegin() const -> const_iterator { return m_coefficients.cbegin(); }
    [[nodiscard]] auto cend() const -> const_iterator { return m_coefficients.cend(); }
    //! \brief Returns the filter number of coefficients.
    [[nodiscard]] auto size() const -> size_type { return m_coefficients.size(); }
    //! \brief Returns the filter minimal indice in its temporal form.
    [[nodiscard]] auto getMinimalIndex() const -> int;
    //! \brief Returns the filter maximal indice in its temporal form.
    [[nodiscard]] auto getMaximalIndex() const -> int;
    //! \brief Returns the filter minimal order in Z transform form.
    [[nodiscard]] auto getMinimalOrder() const -> int;
    //! \brief Returns the filter maximal order in Z transform form.
    [[nodiscard]] auto getMaximalOrder() const -> int;
};

//! \brief Class implementing a basic Infinite Impulse Response (IIR) filter.
class IIR
{
public:
    using value_type = std::vector<double>::value_type;
    using reference = std::vector<double>::reference;
    using const_reference = std::vector<double>::const_reference;
    using iterator = std::vector<double>::iterator;
    using const_iterator = std::vector<double>::const_iterator;
    using difference_type = std::vector<double>::difference_type;
    using size_type = std::vector<double>::size_type;
    using container_type = IIR;

private:
    std::vector<double> m_A, m_B;

public:
    //! \brief Overloaded constructor. Builds an uninitialized filter whose numerator (b <-> X) and denominator (a <->
    //! Y) of the transfer function have the sizes given as input.
    IIR(unsigned a, unsigned b): m_A(a + 1), m_B(b) { m_A[0] = 0.; }
    //! \brief Overloaded constructor. Builds an array containing the values given as inputs for the numerator (b <-> X)
    //! and denominator (a <-> Y).
    IIR(const std::initializer_list<double> &a, const std::initializer_list<double> &b): m_A(a.size() + 1), m_B(b)
    {
        m_A[0] = 0.;
        std::copy(a.begin(), a.end(), m_A.begin() + 1);
    }
    ~IIR() = default;
    IIR(const IIR &f) = default;
    IIR(IIR &&f) = default;
    auto operator=(const IIR &f) -> IIR & = default;
    auto operator=(IIR &&f) -> IIR & = default;
    auto num() -> std::vector<double> & { return m_B; }
    [[nodiscard]] auto num() const -> const std::vector<double> & { return m_B; }
    auto den() -> std::vector<double> & { return m_A; }
    [[nodiscard]] auto den() const -> const std::vector<double> & { return m_A; }
};

//! \brief Returns a new complex array equal to the forward / backward Fast Fourier Transform of the array given as
//! parameter.
template<typename T>
struct fft;

template<>
struct fft<double>
{
    template<typename Iterator1, typename Iterator2>
    static void forward(Iterator1 first, Iterator1 last, Iterator2 result);
    template<typename Iterator1, typename Iterator2>
    static void backward(Iterator1 first, Iterator1 last, Iterator2 result);
};

template<>
struct fft<std::complex<double>>
{
    template<typename Iterator1, typename Iterator2>
    static void forward(Iterator1 first, Iterator1 last, Iterator2 result);
    template<typename Iterator1, typename Iterator2>
    static void backward(Iterator1 first, Iterator1 last, Iterator2 result);
};

//! \brief Computes the convolution of f by h and returns the results. Size of output is equal to the sum of input
//! sizes. The last element is always 0.
template<typename Iterator1, typename Iterator2, typename Iterator3>
void conv(Iterator1 f1, Iterator1 f2, Iterator2 h1, Iterator2 h2, Iterator3 g);
//! \brief Filters the signal x by the FIR filter f in a mode given as parameter. Mode = 'Z' : the signal is padded with
//! zero for negative and posterior times. Mode = 'P' : the signal is periodized with a period equal to its original
//! size.
template<typename Iterator1, typename Iterator2>
void filter(Iterator1 first, Iterator1 last, const FIR &f, Iterator2 out, char mode = 'P');
//! \brief Filters the signal x by the IIR filter f. The signal is padded with zero for negative and posterior times.
template<typename Iterator1, typename Iterator2>
void filter(Iterator1 x1, Iterator1 x2, Iterator2 y1, Iterator2 y2, const IIR &f);
//! \brief Zero-phase forward and reverse digital filtering of the signal x with the FIR f.
template<typename Iterator1, typename Iterator2>
void filtfilt(Iterator1 first, Iterator1 last, const FIR &f, Iterator2 out);
//! \brief Computes the cross-correlation between f and h. Size of output is equal to the sum of input sizes. The last
//! element is always 0. Three modes are available: 'R' for raw (default), 'B' for biaised and 'U' for unbiaised.
template<typename Iterator1, typename Iterator2, typename Iterator3>
void xcorr(Iterator1 f1, Iterator1 f2, Iterator2 h1, Iterator2 h2, Iterator3 out, char mode = 'R');
//! \brief Computes the auto-correlation of f. Size of output is equal to two times the size of f. The last element is
//! always 0. Three modes are available: 'R' for raw (default), 'B' for biaised and 'U' for unbiaised.
template<typename Iterator1, typename Iterator2>
void xcorr(Iterator1 first, Iterator1 last, Iterator2 out, char mode = 'R');
//! \brief Computes the covariance between f and h. Size of output is equal to the sum of input sizes. The last element
//! is always 0. Three modes are available: 'R' for raw (default), 'B' for biaised and 'U' for unbiaised.
template<typename Iterator1, typename Iterator2, typename Iterator3>
void xcov(Iterator1 f1, Iterator1 f2, Iterator2 h1, Iterator2 h2, Iterator3 out, char mode = 'R');
//! \brief Computes the auto-covariance of f. Size of output is equal to two times the size of f. The last element is
//! always 0. Three modes are available: 'R' for raw (default), 'B' for biaised and 'U' for unbiaised.
template<typename Iterator1, typename Iterator2>
void xcov(Iterator1 f1, Iterator1 f2, Iterator2 out, char mode = 'R');
//! \brief Designs a IIR digital filter whose coefficients match the AR(p) process given as input (yule-walker method).
template<typename Iterator>
auto aryule(unsigned p, Iterator first, Iterator last, typename Iterator::value_type *sigma = nullptr) -> IIR;
//! \brief Computes and returns a gaussian filter of size N and standard deviation sigma.
auto gaussian(unsigned N, double sigma) -> std::vector<double>;
//! \brief Computes and returns a hanning windows of size N.
auto hamming(unsigned N, char mode = 'S') -> std::vector<double>;
//! \brief Computes and returns a hamming windows of size N.
auto hanning(unsigned N, char mode = 'S') -> std::vector<double>;
} // namespace iloj::math

#include "filtering.hpp"
