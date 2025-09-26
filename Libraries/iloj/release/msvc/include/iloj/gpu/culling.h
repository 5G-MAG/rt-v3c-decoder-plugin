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

#include "functions.h"

namespace iloj::gpu
{
namespace Culling
{
class Context
{
private:
    GLenum m_mode = GL_NONE;

public:
    explicit Context(GLenum mode = GL_NONE): m_mode(mode)
    {
        if (m_mode != 0U)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(m_mode);
        }
    }
    Context(const Context &) = default;
    Context(Context &&) = default;
    auto operator=(const Context &) -> Context & = default;
    auto operator=(Context &&) -> Context & = default;
    ~Context()
    {
        if (m_mode != 0U)
        {
            glDisable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        }
    }
    static auto None() -> Context { return Context(); }
    static auto Back() -> Context { return Context{GL_BACK}; }
    static auto Front() -> Context { return Context{GL_FRONT}; }
    static auto Both() -> Context { return Context{GL_FRONT_AND_BACK}; }
};
} // namespace Culling
} // namespace iloj::gpu
