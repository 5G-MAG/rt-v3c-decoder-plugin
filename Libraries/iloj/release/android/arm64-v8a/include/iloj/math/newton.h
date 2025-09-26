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

#include <functional>
#include <utility>

namespace iloj::math
{
class Newton
{
public:
    class Output
    {
    protected:
        double m_x;
        double m_f;
        double m_df;
        unsigned m_iter;

    public:
        Output(double x, double f, double df, unsigned iter): m_x(x), m_f(f), m_df(df), m_iter(iter) {}
        [[nodiscard]] auto getOptimum() const -> double { return m_x; }
        [[nodiscard]] auto getValue() const -> double { return m_f; }
        [[nodiscard]] auto getDerivative() const -> double { return m_df; }
        [[nodiscard]] auto getNumberOfIterations() const -> unsigned { return m_iter; }
    };

public:
    static auto solve(const std::function<std::pair<double, double>(double)> &fdf,
                      double x0 = 1.,
                      double feps = 1e-6,
                      double deps = 1e-6,
                      unsigned iter = 10) -> Output;
};

} // namespace iloj::math
