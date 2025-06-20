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

namespace iloj::gpu
{
namespace Depth
{
class Context
{
private:
    GLenum m_depthFunc = GL_NONE;
    bool m_writeOn = false;

public:
    Context(GLenum depthFunc, bool writeOn): m_depthFunc(depthFunc), m_writeOn(writeOn)
    {
        if (m_depthFunc != 0U)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(m_depthFunc);
        }

        if (!m_writeOn)
        {
            glDepthMask(GL_FALSE);
        }
    }
    Context(const Context &) = default;
    Context(Context &&) = default;
    auto operator=(const Context &) -> Context & = default;
    auto operator=(Context &&) -> Context & = default;
    ~Context()
    {
        if (m_depthFunc != 0U)
        {
            glDisable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
        }

        if (!m_writeOn)
        {
            glDepthMask(GL_TRUE);
        }
    }
    static auto None() -> Context { return {GL_NONE, false}; }
    static auto Never(bool writeOn = true) -> Context { return {GL_NEVER, writeOn}; }
    static auto Less(bool writeOn = true) -> Context { return {GL_LESS, writeOn}; }
    static auto Equal(bool writeOn = true) -> Context { return {GL_EQUAL, writeOn}; }
    static auto LEqual(bool writeOn = true) -> Context { return {GL_LEQUAL, writeOn}; }
    static auto NotEqual(bool writeOn = true) -> Context { return {GL_NOTEQUAL, writeOn}; }
    static auto GEqual(bool writeOn = true) -> Context { return {GL_GEQUAL, writeOn}; }
    static auto Greater(bool writeOn = true) -> Context { return {GL_GREATER, writeOn}; }
    static auto Always(bool writeOn = true) -> Context { return {GL_ALWAYS, writeOn}; }
};
} // namespace Depth
} // namespace iloj::gpu
