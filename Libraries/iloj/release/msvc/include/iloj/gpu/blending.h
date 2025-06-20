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

#include "functions.h"
#include <vector>

namespace iloj::gpu
{
namespace Blending
{
class Configuration
{
private:
    GLenum m_equation = GL_NONE;
    GLenum m_sFactor = GL_NONE;
    GLenum m_dFactor = GL_NONE;
    GLenum m_bOperator = GL_NONE;

public:
    Configuration() = default;
    Configuration(GLenum equation, GLenum sFactor, GLenum dFactor, GLenum bOperator)
        : m_equation(equation), m_sFactor(sFactor), m_dFactor(dFactor), m_bOperator(bOperator)
    {
    }
    [[nodiscard]] auto isEnabled() const -> bool { return ((m_equation != GL_NONE) || (m_bOperator != GL_NONE)); }
    [[nodiscard]] auto getEquation() const -> GLenum { return m_equation; }
    [[nodiscard]] auto getSourceFactor() const -> GLenum { return m_sFactor; }
    [[nodiscard]] auto getDestinationFactor() const -> GLenum { return m_dFactor; }
    [[nodiscard]] auto getBooleanOperator() const -> GLenum { return m_bOperator; }
    static auto None() -> Configuration { return {}; }
    static auto Alpha() -> Configuration { return {GL_FUNC_ADD, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_NONE}; }
    static auto Min() -> Configuration { return {GL_MIN, GL_ONE, GL_ONE, GL_NONE}; }
    static auto Max() -> Configuration { return {GL_MAX, GL_ONE, GL_ONE, GL_NONE}; }
    static auto Sum() -> Configuration { return {GL_FUNC_ADD, GL_ONE, GL_ONE, GL_NONE}; }
    static auto And() -> Configuration { return {GL_NONE, GL_NONE, GL_NONE, GL_AND}; }
    static auto Or() -> Configuration { return {GL_NONE, GL_NONE, GL_NONE, GL_OR}; }
    static auto XOr() -> Configuration { return {GL_NONE, GL_NONE, GL_NONE, GL_XOR}; }
    static auto AlphaSaturate() -> Configuration { return {GL_FUNC_ADD, GL_SRC_ALPHA_SATURATE, GL_ONE, GL_NONE}; }
};

class Context
{
private:
    enum class Mode
    {
        Disabled,
        Blending,
        Logic
    };
    Mode m_mode = Mode::Disabled;

public:
    Context() = default;
    explicit Context(const Configuration &cfg)
    {
        if (cfg.getEquation() != GL_NONE)
        {
            m_mode = Mode::Blending;
            glEnable(GL_BLEND);
            glBlendEquation(cfg.getEquation());
            glBlendFunc(cfg.getSourceFactor(), cfg.getDestinationFactor());
        }
        else if (cfg.getBooleanOperator() != GL_NONE)
        {
            m_mode = Mode::Logic;
            glEnable(GL_COLOR_LOGIC_OP);
            glLogicOp(cfg.getBooleanOperator());
        }
    }
    explicit Context(const std::vector<Configuration> &cfgs): m_mode(Mode::Blending)
    {
        for (std::size_t i = 0U; i < cfgs.size(); i++)
        {
            const Configuration &cfg = cfgs[i];

            if (cfg.getEquation() != GL_NONE)
            {
                glEnablei(GL_BLEND, i);
                glBlendEquationi(i, cfg.getEquation());
                glBlendFunci(i, cfg.getSourceFactor(), cfg.getDestinationFactor());
            }
        }
    }
    Context(const Context &) = default;
    Context(Context &&) = default;
    auto operator=(const Context &) -> Context & = default;
    auto operator=(Context &&) -> Context & = default;
    ~Context()
    {
        if (m_mode == Mode::Blending)
        {
            glDisable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ZERO);
        }
        else if (m_mode == Mode::Logic)
        {
            glDisable(GL_COLOR_LOGIC_OP);
        }
    }
    static auto None() -> Context { return Context(); }
    static auto Alpha() -> Context { return Context{Configuration::Alpha()}; }
    static auto Min() -> Context { return Context{Configuration::Min()}; }
    static auto Max() -> Context { return Context{Configuration::Max()}; }
    static auto Sum() -> Context { return Context{Configuration::Sum()}; }
    static auto And() -> Context { return Context{Configuration::And()}; }
    static auto Or() -> Context { return Context{Configuration::Or()}; }
    static auto XOr() -> Context { return Context{Configuration::XOr()}; }
    static auto AlphaSaturate() -> Context { return Context{Configuration::AlphaSaturate()}; }
};
} // namespace Blending
} // namespace iloj::gpu
