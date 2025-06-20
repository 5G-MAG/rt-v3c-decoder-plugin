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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename Iterator>
void fft_pow2(Iterator X, int N, const std::complex<double> &w)
{
    if (1 < N)
    {
        int hN = N / 2;

        // Separation in even (first half) / odd (second half) items
        {
            std::vector<std::complex<double>> buffer(hN);

            for (int i = 0; i < hN; i++)
            {
                buffer[i] = X[i * 2 + 1];
            }

            for (int i = 0; i < hN; i++)
            {
                X[i] = X[i * 2];
            }

            std::copy(buffer.begin(), buffer.end(), X + hN);
        }

        // Recursion on even / odd items
        std::complex<double> w2 = w * w;

        fft_pow2(X, hN, w2);
        fft_pow2(X + hN, hN, w2);

        // Result combination of two half recursions
        std::complex<double> wk = 1;

        for (int k = 0; k < hN; k++)
        {
            std::complex<double> e = X[k];
            std::complex<double> o = X[k + hN];

            X[k] = e + wk * o;
            X[k + hN] = e - wk * o;

            wk *= w;
        }
    }
}

template<typename Iterator1, typename Iterator2>
void fft_pow2_forward(Iterator1 first, Iterator1 last, Iterator2 result)
{
    int n = last - first;
    std::copy(first, last, result);
    fft_pow2(result, n, {cos(M_2PI / n), -sin(M_2PI / n)});
}

template<typename Iterator1, typename Iterator2>
void fft_pow2_backward(Iterator1 first, Iterator1 last, Iterator2 result)
{
    int n = last - first;
    std::copy(first, last, result);
    fft_pow2(result, n, {cos(M_2PI / n), sin(M_2PI / n)});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename Iterator1, typename Iterator2>
void czt(Iterator1 first, Iterator1 last, Iterator2 result, const std::complex<double> &w)
{
    int n = last - first;
    unsigned N = nextPow2(2 * n - 1);

    // Chirp computation
    std::vector<std::complex<double>> chirp(2 * n - 1);

    for (int i = 0, k = (1 - n); k < n; i++, k++)
    {
        chirp[i] = std::pow(w, k * k / 2.);
    }

    // Input padded
    std::vector<std::complex<double>> x(N);
    std::vector<std::complex<double>> X(N);

    std::transform(first,
                   last,
                   chirp.begin() + (n - 1),
                   x.begin(),
                   [](const std::complex<double> &c1, const std::complex<double> &c2) { return (c1 * c2); });
    std::fill(x.begin() + n, x.end(), 0);

    fft_pow2_forward(x.begin(), x.end(), X.begin());

    // Inverse chirp padded
    std::vector<std::complex<double>> ichirpp(N);
    std::vector<std::complex<double>> ICHIRPP(N);

    std::transform(chirp.begin(), chirp.end(), ichirpp.begin(), [](const std::complex<double> &c) { return (1. / c); });
    std::fill(ichirpp.begin() + chirp.size(), ichirpp.end(), 0);

    fft_pow2_forward(ichirpp.begin(), ichirpp.end(), ICHIRPP.begin());

    // Output
    std::vector<std::complex<double>> R(N);
    std::vector<std::complex<double>> r(N);

    std::transform(X.begin(),
                   X.end(),
                   ICHIRPP.begin(),
                   R.begin(),
                   [](const std::complex<double> &c1, const std::complex<double> &c2) { return (c1 * c2); });
    fft_pow2_backward(R.begin(), R.end(), r.begin());
    std::transform(
        r.begin() + (n - 1),
        r.begin() + (2 * n - 1),
        chirp.begin() + (n - 1),
        result,
        [N](const std::complex<double> &c1, const std::complex<double> &c2) { return (c1 * c2) / double(N); });
}

template<typename Iterator1, typename Iterator2>
void czt_forward(Iterator1 first, Iterator1 last, Iterator2 result)
{
    int n = last - first;
    czt(first, last, result, {cos(M_2PI / n), -sin(M_2PI / n)});
}

template<typename Iterator1, typename Iterator2>
void czt_backward(Iterator1 first, Iterator1 last, Iterator2 result)
{
    int n = last - first;
    czt(first, last, result, {cos(M_2PI / n), sin(M_2PI / n)});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename Iterator1,
         typename Iterator2,
         typename std::enable_if_t<
             std::is_same<typename std::iterator_traits<Iterator1>::value_type, std::complex<double>>::value,
             int> = 0>
void fft_forward_imp(Iterator1 first, Iterator2 last, Iterator2 result)
{
    unsigned n = last - first;
    unsigned N = nextPow2(n);

    if (n != N)
    {
        czt_forward(first, last, result);
    }
    else
    {
        fft_pow2_forward(first, last, result);
    }
}

template<typename Iterator1,
         typename Iterator2,
         typename std::enable_if_t<std::is_same<typename std::iterator_traits<Iterator1>::value_type, double>::value,
                                   int> = 0>
void fft_forward_imp(Iterator1 first, Iterator1 last, Iterator2 result)
{
    std::vector<std::complex<double>> input(first, last);
    fft_forward_imp(input.begin(), input.end(), result);
}

template<typename Iterator1,
         typename Iterator2,
         typename std::enable_if_t<
             std::is_same<typename std::iterator_traits<Iterator2>::value_type, std::complex<double>>::value,
             int> = 0>
void fft_backward_imp(Iterator1 first, Iterator1 last, Iterator2 result)
{
    unsigned n = last - first;
    unsigned N = nextPow2(n);

    if (n != N)
    {
        czt_backward(first, last, result);
    }
    else
    {
        fft_pow2_backward(first, last, result);
    }

    std::for_each(result, result + n, [n](std::complex<double> &x) { x /= n; });
}

template<typename Iterator1,
         typename Iterator2,
         typename std::enable_if_t<std::is_same<typename std::iterator_traits<Iterator2>::value_type, double>::value,
                                   int> = 0>
void fft_backward_imp(Iterator1 first, Iterator1 last, Iterator2 result)
{
    std::vector<std::complex<double>> output(last - first);
    fft_backward_imp(first, last, output.begin());
    std::transform(output.begin(), output.end(), result, [](const std::complex<double> &x) { return x.real(); });
}
} // namespace detail

template<typename Iterator1, typename Iterator2>
void fft<double>::forward(Iterator1 first, Iterator1 last, Iterator2 result)
{
    detail::fft_forward_imp(first, last, result);
}

template<typename Iterator1, typename Iterator2>
void fft<double>::backward(Iterator1 first, Iterator1 last, Iterator2 result)
{
    detail::fft_backward_imp(first, last, result);
}

template<typename Iterator1, typename Iterator2>
void fft<std::complex<double>>::forward(Iterator1 first, Iterator1 last, Iterator2 result)
{
    detail::fft_forward_imp(first, last, result);
}

template<typename Iterator1, typename Iterator2>
void fft<std::complex<double>>::backward(Iterator1 first, Iterator1 last, Iterator2 result)
{
    detail::fft_backward_imp(first, last, result);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace detail
{
template<typename Iterator1, typename Iterator2, typename Iterator3>
void conv1(Iterator1 f, const int L, Iterator2 h, const int M, Iterator3 g)
{
    // Convolution classique
    int N = L + M;

    std::fill(g, g + N, 0.);

    for (int n = 0; n < N; n++)
    {
        const int k = std::min(L, n + 1);

        for (int i = std::max(0, n - M + 1); i < k; i++)
        {
            g[n] += f[i] * h[n - i];
        }
    }
}

template<typename Iterator1, typename Iterator2, typename Iterator3>
void conv2(Iterator1 f, Iterator2 h, const int M, Iterator3 g)
{
    // Convolution FFT (dimensions identiques)
    int N = 2 * M;
    std::vector<double> ff(N);
    std::vector<double> hh(N);
    std::vector<std::complex<double>> FF(N);
    std::vector<std::complex<double>> HH(N);
    std::vector<std::complex<double>> G(N);

    std::copy(f, f + M, ff.begin());
    std::copy(h, h + M, hh.begin());

    std::fill(ff.begin() + M, ff.end(), 0.);
    std::fill(hh.begin() + M, hh.end(), 0.);

    fft<double>::forward(ff.begin(), ff.end(), FF.begin());
    fft<double>::forward(hh.begin(), hh.end(), HH.begin());

    std::transform(
        FF.begin(), FF.end(), HH.begin(), G.begin(), [](const std::complex<double> &a, const std::complex<double> &b) {
            return a * b;
        });

    fft<double>::backward(G.begin(), G.end(), g);
}

template<typename Iterator1, typename Iterator2, typename Iterator3>
void conv3(Iterator1 f, const int L, Iterator2 h, const int M, Iterator3 g)
{
    // Convolution Rapide par Blocs classique ou FFT (cas M<L)
    int N = 2 * M;
    int i = 0;
    int j = 0;
    int k = 0;
    int n = 0;

    std::fill(g, g + (L + M), 0.);

    if (L <= 64)
    {
        std::vector<double> fr(M);
        std::vector<double> gr(N);

        for (i = 0, n = 0; i <= (L / M); i++, n += M)
        {
            for (j = 0, k = n; (j < M) && (k < L); j++, k++)
            {
                fr[j] = f[k];
            }
            for (; j < M; j++)
            {
                fr[j] = 0.;
            }

            conv1(fr.begin(), M, h, M, gr.begin());

            for (j = 0, k = n; (j < N) && (k < (M + L)); j++, k++)
            {
                g[k] += gr[j];
            }
        }
    }
    else
    {
        std::vector<double> fr(N);
        std::vector<double> hh(N);
        std::vector<double> gr(N);
        std::vector<std::complex<double>> FR(N);
        std::vector<std::complex<double>> HH(N);
        std::vector<std::complex<double>> GR(N);

        // Construction de hh
        std::copy(h, h + M, hh.begin());
        std::fill(hh.begin() + M, hh.end(), 0.);

        fft<double>::forward(hh.begin(), hh.end(), HH.begin());

        // Boucle sur fr
        for (i = 0, n = 0; i <= (L / M); i++, n += M)
        {
            for (j = 0, k = n; (j < M) && (k < L); j++, k++)
            {
                fr[j] = f[k];
            }
            for (; j < N; j++)
            {
                fr[j] = 0.;
            }

            fft<double>::forward(fr.begin(), fr.end(), FR.begin());
            std::transform(FR.begin(),
                           FR.end(),
                           HH.begin(),
                           GR.begin(),
                           [](const std::complex<double> &a, const std::complex<double> &b) { return a * b; });
            fft<double>::backward(GR.begin(), GR.end(), gr.begin());

            for (j = 0, k = n; (j < N) && (k < (M + L)); j++, k++)
            {
                g[k] += gr[j];
            }
        }
    }
}
} // namespace detail

template<typename Iterator1, typename Iterator2, typename Iterator3>
void conv(Iterator1 f1, Iterator1 f2, Iterator2 h1, Iterator2 h2, Iterator3 g)
{
    int L = f2 - f1;
    int M = h2 - h1;

    if (M == L)
    {
        if (M <= 64)
        {
            // Convolution classique (dimensions identiques)
            detail::conv1(f1, M, h1, M, g);
        }
        else
        {
            // Convolution FFT (dimensions identiques)
            detail::conv2(f1, h1, M, g);
        }
    }
    else
    {
        if (L < M)
        {
            // Convolution Rapide par Blocs FFT (cas L < M)
            detail::conv3(h1, M, f1, L, g);
        }
        else
        {
            // Convolution Rapide par Blocs FFT (cas M <= L)
            detail::conv3(f1, L, h1, M, g);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename Iterator1, typename Iterator2>
void filter(Iterator1 first, Iterator1 last, const FIR &f, Iterator2 out, char mode)
{
    int i = 0;
    int j = 0;
    int n = last - first;
    int xhmin = 0;
    int xhmax = 0;
    int ix = 0;
    std::vector<double> tmp;

    if (mode == 'Z')
    {
        tmp.resize(n + f.size());

        // Signal x padded with zero for negative and posterior times.
        xhmin = f.getMinimalOrder() - (n - 1);
        xhmax = f.getMaximalOrder();

        conv(first, last, f.cbegin(), f.cend(), tmp.begin());
    }
    else if (mode == 'P')
    {
        // Signal x periodized with a period equal to lenght of x.
        int xmin = -(f.getMaximalOrder() + (n - 1));
        int xmax = -(f.getMinimalOrder());
        std::vector<double> xpadded(xmax - xmin + 1);

        tmp.resize(xpadded.size() + f.size());

        xhmin = (f.getMinimalOrder() - f.getMaximalOrder()) - (n - 1);
        xhmax = (f.getMaximalOrder() - f.getMinimalOrder());

        // Building x periodized
        for (ix = xmax, i = 0, j = (n - (xmax % n)) % n; xmin <= ix; ix--, i++, j = ((j != (n - 1)) ? j + 1 : 0))
        {
            xpadded[i] = first[j];
        }

        conv(xpadded.begin(), xpadded.end(), f.cbegin(), f.cend(), tmp.begin());
    }

    // Building output
    if (0 <= xhmax)
    {
        i = xhmax;
        j = 0;
        ix = 0;
    }
    else
    {
        i = 0;
        for (j = 0, ix = 0; xhmax < ix; j++, ix--)
        {
            out[j] = 0;
        }
    }

    for (; ((xhmin <= ix) && (j < n)); i++, ix--, j++)
    {
        out[j] = tmp[i];
    }

    for (; j < n; j++)
    {
        out[j] = 0;
    }
}

template<typename Iterator1, typename Iterator2>
void filter(Iterator1 x1, Iterator1 x2, Iterator2 y1, Iterator2 y2, const IIR &f)
{
    using T2 = typename std::iterator_traits<Iterator2>::value_type;

    int sz = 0;
    int n = x2 - x1;
    int m = y2 - y1;
    T2 out;

    for (int t = m; t < n; t++)
    {
        out = 0;

        // Numerator
        sz = (t + 1) - std::max(0, (int) ((t + 1) - f.num().size()));
        out += std::inner_product(
            f.num().begin(), f.num().begin() + sz, std::reverse_iterator<Iterator1>(x1 + t + 1), (T2) 0);

        // Denominator
        sz = (t + 1) - std::max(0, (int) ((t + 1) - f.den().size()));
        out -= std::inner_product(
            f.den().begin(), f.den().begin() + sz, std::reverse_iterator<Iterator2>(y1 + t + 1), (T2) 0);

        y1[t] = out;
    }
}

template<typename Iterator1, typename Iterator2>
void filtfilt(Iterator1 first, Iterator1 last, const FIR &f, Iterator2 out)
{
    int n = last - first;
    std::vector<double> tmp(n);

    filter(first, last, f, tmp.begin());
    std::reverse_copy(tmp.begin(), tmp.end(), out);
    filter(out, out + n, f, tmp.begin());
    std::reverse_copy(tmp.begin(), tmp.end(), out);
}

template<typename Iterator1, typename Iterator2, typename Iterator3>
void xcorr(Iterator1 f1, Iterator1 f2, Iterator2 h1, Iterator2 h2, Iterator3 out, char mode)
{
    int N = f2 - f1;
    int m = h2 - h1;
    std::vector<typename std::iterator_traits<Iterator1>::value_type> fr(f1, f2);

    std::reverse(fr.begin(), fr.end());
    conv(fr.cbegin(), fr.cend(), h1, h2, out);

    if (mode == 'B')
    {
        std::transform(
            out, out + (N + m), out, [N](typename std::iterator_traits<Iterator3>::value_type &x) { return x /= N; });
    }
    else if (mode == 'U')
    {
        out[N - 1] /= N;
        for (int i = N, j = N - 2, k = 1; k < N; k++, i++, j--)
        {
            out[i] /= (N - k);
            out[j] = out[i];
        }
    }
}

template<typename Iterator1, typename Iterator2>
void xcorr(Iterator1 first, Iterator1 last, Iterator2 out, char mode)
{
    xcorr(first, last, first, last, out, mode);
}

template<typename Iterator1, typename Iterator2, typename Iterator3>
void xcov(Iterator1 f1, Iterator1 f2, Iterator2 h1, Iterator2 h2, Iterator3 out, char mode)
{
    int N = f2 - f1;
    int m = h2 - h1;

    using T1 = typename std::iterator_traits<Iterator1>::value_type;
    using T2 = typename std::iterator_traits<Iterator2>::value_type;

    // Copy
    std::vector<T1> sf(f1, f2);
    std::vector<T2> sh(h1, h2);

    // Mean estimation
    T1 mf = std::accumulate(sf.begin(), sf.end(), (T1) 0) / N;
    T2 mh = std::accumulate(sh.begin(), sh.end(), (T2) 0) / m;

    // Centering
    for (auto &v : sf)
    {
        v -= mf;
    }
    for (auto &v : sh)
    {
        v -= mh;
    }

    // Cross-correlation
    xcorr(sf.begin(), sf.end(), h1, h2, out, mode);
}

template<typename Iterator1, typename Iterator2>
void xcov(Iterator1 f1, Iterator1 f2, Iterator2 out, char mode)
{
    using T1 = typename std::iterator_traits<Iterator1>::value_type;

    int N = f2 - f1;

    // Copy
    std::vector<T1> sf(f1, f2);

    // Mean estimation
    T1 mf = std::accumulate(sf.begin(), sf.end(), (T1) 0) / N;

    // Centering
    for (auto &v : sf)
    {
        v -= mf;
    }

    // Cross-correlation
    xcorr(sf.begin(), sf.end(), out, mode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename Iterator>
auto aryule(unsigned p, Iterator first, Iterator last, typename Iterator::value_type *sigma) -> IIR
{
    using T = typename std::iterator_traits<Iterator>::value_type;

    // Covariance
    unsigned n = last - first;
    unsigned i0 = n - 1;
    std::vector<T> cv(2 * n);
    xcov(first, last, cv.begin(), 'B');

    // Build sytem
    Mat<T> M({p + 1, p + 1});
    Vec<T> b({p + 1});

    for (unsigned i = 0; i <= p; i++)
    {
        // Right hand side vector
        b[i] = cv[i0 + i];

        // Toeplitz matrix
        auto iter = M.row_begin(i);

        for (unsigned j = 0; j < p; j++, iter++)
        {
            *iter = cv[i0 + std::abs(int(i) - int(j + 1))];
        }

        *iter = (i ? 0 : 1);
    }

    // Solve system
    auto a = mldivide(M, b);

    // Output
    IIR out(p, 1);

    std::transform(a.begin(), a.begin() + p, out.den().begin() + 1, [](T v) { return -v; });
    out.num()[0] = 1;

    if (sigma)
    {
        *sigma = a[p];
    }

    return out;
}
} // namespace iloj::math
