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

#include "color.h"
#include "texture.h"
#include "viewport.h"

namespace iloj::gpu
{
namespace Clear
{
class Context
{
private:
    std::vector<GLColor> m_clearColor;
    float m_clearDepth{-1.F};

public:
    Context() = default;
    ~Context() = default;
    Context(std::vector<GLColor> clearColor, float clearDepth)
        : m_clearColor(std::move(clearColor)), m_clearDepth(clearDepth)
    {
    }
    Context(const Context &) = default;
    Context(Context &&) = default;
    auto operator=(const Context &) -> Context & = default;
    auto operator=(Context &&) -> Context & = default;
    template<typename T>
    void apply(TargetList<T> targets, const ViewPort &viewPort) const
    {
        if (!(hasClearColor() || hasClearDepth()))
        {
            return;
        }

        glEnable(GL_SCISSOR_TEST);

        glScissor(viewPort.getLeft(), viewPort.getBottom(), viewPort.getWidth(), viewPort.getHeight());

        if (hasClearColor())
        {
            if (targets.empty())
            {
                glClearColor(m_clearColor[0][0], m_clearColor[0][1], m_clearColor[0][2], m_clearColor[0][3]);
                glClear(GL_COLOR_BUFFER_BIT);
            }
            else
            {
                std::size_t id = 0;

                for (const auto &v : targets)
                {
                    unsigned colorId = std::min(id, m_clearColor.size() - 1);

                    if (v->isIntegerType())
                    {
                        Vec4i c{m_clearColor[colorId]};
                        glClearBufferiv(GL_COLOR, id, c.data());
                    }
                    else if (v->isUnsignedIntegerType())
                    {
                        Vec4u c{m_clearColor[colorId]};
                        glClearBufferuiv(GL_COLOR, id, c.data());
                    }
                    else
                    {
                        glClearBufferfv(GL_COLOR, id, m_clearColor[0].data());
                    }

                    id++;
                }
            };
        }

        if (hasClearDepth())
        {
            glClearDepthf(m_clearDepth);
            glClear(GL_DEPTH_BUFFER_BIT);
        }

        glDisable(GL_SCISSOR_TEST);
    }
    [[nodiscard]] auto hasClearColor() const -> bool { return !m_clearColor.empty(); }
    [[nodiscard]] auto hasClearDepth() const -> bool { return (-1.F < m_clearDepth); }
    static auto None() -> Context { return Context({}, -1.F); }
    static auto Color(std::vector<GLColor> clearColor = {Color::None()}) -> Context
    {
        return Context(std::move(clearColor), -1.F);
    }
    static auto Depth(float clearDepth = 1.F) -> Context { return Context({}, clearDepth); }
    static auto All(std::vector<GLColor> clearColor = {Color::None()}, float clearDepth = 1.F) -> Context
    {
        return Context(std::move(clearColor), clearDepth);
    }
};
} // namespace Clear
} // namespace iloj::gpu
