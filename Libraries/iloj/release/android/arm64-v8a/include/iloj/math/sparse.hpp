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

#include "sparse.h"

namespace iloj::math::sparse
{
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename MAT>
auto LSQR<MAT>::Status::getMessage() const -> const std::string &
{
    static const std::array<std::string, 8> messageList = {"The exact solution is  x = 0",
                                                           "Ax - b is small enough, given atol, btol",
                                                           "The least-squares solution is good enough, given atol",
                                                           "The estimate of cond(Abar) has exceeded conlim",
                                                           "Ax - b is small enough for this machine",
                                                           "The least-squares solution is good enough for this machine",
                                                           "Cond(Abar) seems to be too large for this machine",
                                                           "The iteration limit has been reached"};

    return messageList[m_code];
}

template<typename MAT>
auto LSQR<MAT>::solve(const Matrix &A, const Vector &b, value_type damp, Vector &x) const -> Status
{
    // https://web.stanford.edu/group/SOL/software/lsqr/
    x.resize(A.n(), true);

    // Set up the first vectors u and v for the bidiagonalization.
    // These satisfy  beta*u = b,  alpha*v = A'u.
    Vector u = b;
    Vector v{{A.n(), true}};
    Vector w{{A.n(), true}};
    value_type alpha{};
    value_type beta = norm(u);

    if (0 < beta)
    {
        scale(1 / beta, u, u);
        matprod(value_type{1}, A, 'T', u, value_type{0}, v);
        alpha = norm(v);
    }

    if (0 < alpha)
    {
        scale(1 / alpha, v, v);
        w = v;
    }

    if ((0 < alpha) && (0 < beta))
    {
        // Initialize
        value_type ctol = (0 < m_parameter.getConditionNumberLimit()) ? (1 / m_parameter.getConditionNumberLimit()) : 0;
        value_type anorm{};
        value_type acond{};
        value_type dampsq = damp * damp;
        value_type ddnorm{};
        value_type res2{};
        value_type xnorm{};
        value_type xxnorm{};
        value_type z{};
        value_type cs2{-1};
        value_type sn2{};

        auto rhobar = alpha;
        auto phibar = beta;
        auto bnorm = beta;
        auto rnorm = beta;
        auto r1norm = rnorm;

        // Main iteration loop
        for (auto itn = 0ULL; itn < m_parameter.getMaxNumberOfIteration(); itn++)
        {
            // Perform the next step of the bidiagonalization to obtain the
            // next  beta, u, alpha, v.  These satisfy the relations
            //            beta*u  =  A*v   -  alpha*u,
            //            alpha*v  =  A'*u  -  beta*v.
            matprod(value_type{1}, A, 'N', v, -alpha, u);
            beta = norm(u);

            anorm = std::sqrt(anorm * anorm + alpha * alpha + beta * beta + damp * damp);

            if (0 < beta)
            {
                scale(1 / beta, u, u);
                matprod(value_type{1}, A, 'T', u, -beta, v);
                alpha = norm(v);

                if (0 < alpha)
                {
                    scale(1 / alpha, v, v);
                }
            }

            // Use a plane rotation to eliminate the damping parameter.
            // This alters the diagonal (rhobar) of the lower-bidiagonal matrix.
            auto rhobar1 = std::sqrt(rhobar * rhobar + damp * damp);
            auto cs1 = rhobar / rhobar1;
            auto sn1 = damp / rhobar1;
            auto psi = sn1 * phibar;
            phibar = cs1 * phibar;

            // Use a plane rotation to eliminate the subdiagonal element (beta)
            // of the lower-bidiagonal matrix, giving an upper-bidiagonal matrix.
            auto rho = std::sqrt(rhobar1 * rhobar1 + beta * beta);
            auto cs = rhobar1 / rho;
            auto sn = beta / rho;
            auto theta = sn * alpha;
            rhobar = -cs * alpha;
            auto phi = cs * phibar;
            phibar = sn * phibar;
            auto tau = sn * phi;

            // Update x and w.
            auto t1 = phi / rho;
            auto t2 = -theta / rho;

            ddnorm += (norm2(w) / (rho * rho));

            linear(t1, w, value_type{1}, x, x);
            linear(value_type{1}, v, t2, w, w);

            // Use a plane rotation on the right to eliminate the
            // super-diagonal element (theta) of the upper-bidiagonal matrix.
            // Then use the result to estimate  norm(x).
            auto delta = sn2 * rho;
            auto gambar = -cs2 * rho;
            auto rhs = phi - delta * z;
            auto zbar = rhs / gambar;
            xnorm = std::sqrt(xxnorm + zbar * zbar);
            auto gamma = std::sqrt(gambar * gambar + theta * theta);
            cs2 = gambar / gamma;
            sn2 = theta / gamma;
            z = rhs / gamma;
            xxnorm = xxnorm + z * z;

            // Test for convergence.
            // First, estimate the condition of the matrix  Abar,
            // and the norms of  rbar  and  Abar'rbar.
            acond = anorm * std::sqrt(ddnorm);
            auto res1 = phibar * phibar;
            res2 = res2 + psi * psi;
            rnorm = std::sqrt(res1 + res2);
            auto arnorm = alpha * std::abs(tau);

            // Distinguish between
            //    r1norm = ||b - Ax|| and
            //    r2norm = rnorm in current code
            //           = sqrt(r1norm^2 + damp^2*||x||^2).
            //    Estimate r1norm from
            //    r1norm = sqrt(r2norm^2 - damp^2*||x||^2).
            // Although there is cancellation, it might be accurate enough.
            auto r1sq = rnorm * rnorm - dampsq * xxnorm;
            r1norm = std::sqrt(std::abs(r1sq));
            if (r1sq < 0)
            {
                r1norm = -r1norm;
            }

            // Now use these norms to estimate certain other quantities,
            // some of which will be small near a solution.
            auto test1 = rnorm / bnorm;
            auto test2 = arnorm / (anorm * (rnorm + value_type{1e-50}));
            auto test3 = 1 / acond;
            t1 = test1 / (1 + anorm * xnorm / bnorm);
            auto rtol =
                m_parameter.getBToleranceThreshold() + m_parameter.getAToleranceThreshold() * anorm * xnorm / bnorm;

            // The following tests guard against extremely small values of
            // atol, btol  or  ctol.  (The user may have set any or all of
            // the parameters  atol, btol, conlim  to 0.)
            // The effect is equivalent to the normal tests using
            // atol = eps,  btol = eps,  conlim = 1/eps.
            if ((1 + test3) <= 1)
            {
                return {itn + 1, rnorm, 6};
            }
            if ((1 + test2) <= 1)
            {
                return {itn + 1, rnorm, 5};
            }
            if ((1 + t1) <= 1)
            {
                return {itn + 1, rnorm, 4};
            }

            // Allow for tolerances set by the user.
            if (test3 <= ctol)
            {
                return {itn + 1, rnorm, 3};
            }
            if (test2 <= m_parameter.getAToleranceThreshold())
            {
                return {itn + 1, rnorm, 2};
            }
            if (test1 <= rtol)
            {
                return {itn + 1, rnorm, 1};
            }
        }

        return {m_parameter.getMaxNumberOfIteration(), rnorm, 7};
    }

    return {0ULL, value_type{0}, 0};
}

////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename MAT>
auto LM<MAT>::Status::getMessage() const -> const std::string &
{
    static std::array<std::string, 5> messageList = {"The iteration limit has been reached",
                                                     "Objective norm small enough given threshold",
                                                     "Gradient norm small enough given threshold",
                                                     "Step norm small enough given threshold",
                                                     "Improvement big enough given threshold"};

    return messageList[m_code];
}

template<typename MAT>
void LM<MAT>::Status::toStream(std::ostream &os) const
{
    os << "Iterations: " << m_itn << '\n';
    os << "||f||_inf: " << m_fnorm << '\n';
    os << "||g||_inf: " << m_gnorm << '\n';
    os << "Improvement: " << (m_improvement * 100) << "%\n";
    os << "Message: " << getMessage() << '\n';
}

////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename MAT>
auto LM<MAT>::Optimizer::minimize(const CostFunction &costFunction,
                                  Vector &x,
                                  const BoxConstraint &boxConstraint,
                                  const LoggingCallback &onLogging) const -> Status
{
    Vector f;
    Vector g;
    Vector h;
    List list;
    Matrix J;

    costFunction(x, f, list);

    J = Matrix::fromElementList(f.size(), x.size(), list);
    matprod(value_type{1}, J, 'T', f, value_type{0}, g);

    auto nx = norm(x);
    auto nf_inf = norm_inf(f);
    auto nf = norm(f);
    auto ng_inf = norm_inf(g);

    auto nf_0 = nf;
    value_type overallImprovement = 0;

    if (onLogging)
    {
        onLogging(0ULL, overallImprovement, x, f);
    }

    auto iter = 1ULL;
    auto status = Status::MaxIter;

    if (nf_inf <= m_lmParameter.getObjectiveTolerance())
    {
        status = Status::CodeId::ObjectiveTol;
        goto end; // NOLINT
    }
    else if (ng_inf <= m_lmParameter.getGradientTolerance())
    {
        status = Status::CodeId::GradientTol;
        goto end; // NOLINT
    }
    else
    {
        // Initialization (computing max diagonal element of J'J at start for mu first value)
        value_type aii_max = getMaxColumnSquaredNorm(list, static_cast<index_type>(J.n()));
        auto mu = m_lmParameter.getTau() * aii_max;
        value_type nu{2};

        Vector x_new;
        Vector f_new;

        // Main loop
        for (; iter < m_lmParameter.getMaxNumberOfIteration(); iter++)
        {
            auto lsqrStatus = m_lsqrContext.solve(J, f, std::sqrt(mu), h);

            stepbox(x, boxConstraint.first, boxConstraint.second, h);

            auto nh = norm(h);

            if (nh < (m_lmParameter.getStepTolerance() * (nx + m_lmParameter.getStepTolerance())))
            {
                status = Status::CodeId::StepTol;
                goto end; // NOLINT
            }
            else
            {
                sub(x, h, x_new);

                costFunction(x_new, f_new, list);

                auto nf_new = norm(f_new);

                // Check if acceptable step
                auto ksi = (nf * nf - nf_new * nf_new) / (mu * nh * nh + dot(h, g));

                if ((nf_new < nf) && (0 < ksi))
                {
                    x = std::move(x_new);
                    f = std::move(f_new);

                    J = {};
                    J = Matrix::fromElementList(f.size(), x.size(), list);

                    matprod(value_type{1}, J, 'T', f, value_type{0}, g);

                    nx = norm(x);
                    nf_inf = norm_inf(f);
                    nf = nf_new;
                    ng_inf = norm_inf(g);

                    auto oldOverallImprovement = overallImprovement;
                    overallImprovement = getImprovement(nf_0, nf);

                    if (nf_inf <= m_lmParameter.getObjectiveTolerance())
                    {
                        status = Status::CodeId::ObjectiveTol;
                        goto end; // NOLINT
                    }
                    else if (ng_inf <= m_lmParameter.getGradientTolerance())
                    {
                        status = Status::CodeId::GradientTol;
                        goto end; // NOLINT
                    }
                    else if ((overallImprovement - oldOverallImprovement) < m_lmParameter.getImprovementTolerance())
                    {
                        status = Status::CodeId::ImprovementTol;
                        goto end; // NOLINT
                    }
                    else
                    {
                        mu *= std::max(value_type{1} / 3, 1 - cube<value_type>(2 * ksi - 1));
                        nu = 2;
                    }
                }
                else
                {
                    mu *= nu;
                    nu *= 2;
                }

                if (onLogging)
                {
                    onLogging(iter, overallImprovement, x, f);
                }
            }
        }
    }
end:
    if (onLogging)
    {
        onLogging(iter, overallImprovement, x, f);
    }

    return {iter, nf_inf, ng_inf, overallImprovement, status};
}

} // namespace iloj::math::sparse
