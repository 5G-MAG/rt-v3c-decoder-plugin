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

#include "blending.h"
#include "clear.h"
#include "context.h"
#include "culling.h"
#include "depth.h"
#include "mesh.h"

namespace iloj::gpu
{
template<typename T = Texture2D>
void execute(TargetList<T> targets, const std::function<void()> &plan)
{
    switch (targets.size())
    {
        case 0:
            plan();
            break;
        case 1:
            Context::getInstance().execute(targets.begin()->get(), plan);
            break;
        default:
            Context::getInstance().execute(targets, plan);
    }
}

template<typename T = Texture2D, typename... ENTRIES>
void execute(TargetList<T> targets,
             const ViewPort &viewPort,
             const Clear::Context &clearContext,
             const Blending::Context & /*unused*/,
             const Depth::Context & /*unused*/,
             const Culling::Context & /*unused*/,
             const std::function<void()> &plan)
{
    auto meta_plan = [&]() {
        viewPort.apply();
        clearContext.apply(targets, viewPort);

        plan();
    };

    execute(targets, meta_plan);
}

template<typename T = Texture2D, typename... ENTRIES>
void execute(TargetList<T> targets,
             const ViewPort &viewPort,
             const Clear::Context &clearContext,
             const Blending::Context & /*unused*/,
             const Depth::Context & /*unused*/,
             const Culling::Context & /*unused*/,
             Drawable &drawable,
             Program &program,
             ENTRIES... entries)
{
    auto plan = [&]() {
        viewPort.apply();
        clearContext.apply(targets, viewPort);

        program.bind();

        program.setUniform(std::forward<ENTRIES>(entries)...);

        drawable.draw(program);

        iloj::gpu::Program::unbind();
    };

    execute(targets, plan);
}

template<typename T = Texture2D, typename... ENTRIES>
void execute(TargetList<T> targets,
             const ViewPort &viewPort,
             const Clear::Context &clearContext,
             const Blending::Context & /*unused*/,
             const Depth::Context & /*unused*/,
             const Culling::Context & /*unused*/,
             Drawable &drawable,
             int instanceCount,
             Program &program,
             ENTRIES... entries)
{
    auto plan = [&]() {
        viewPort.apply();
        clearContext.apply(targets, viewPort);

        program.bind();

        program.setUniform(std::forward<ENTRIES>(entries)...);
        drawable.draw(program, instanceCount);

        iloj::gpu::Program::unbind();
    };

    execute(targets, plan);
}

template<typename T = Texture2D, typename... ENTRIES>
void execute(TargetList<T> targets,
             const ViewPort &viewPort,
             const Clear::Context &clearContext,
             const Blending::Context & /*unused*/,
             const Depth::Context & /*unused*/,
             const Culling::Context & /*unused*/,
             int instanceCount,
             Program &program,
             ENTRIES... entries)
{
    static Drawable drawable = Mesh::PointIndex(1).toDrawable();

    auto plan = [&]() {
        viewPort.apply();
        clearContext.apply(targets, viewPort);

        program.bind();

        program.setUniform(std::forward<ENTRIES>(entries)...);
        drawable.draw(program, instanceCount);

        iloj::gpu::Program::unbind();
    };

    execute(targets, plan);
}

template<typename... ENTRIES>
void execute(const Vec3u &numGroups, unsigned barriers, Program &program, ENTRIES... entries)
{
    program.bind();

    program.setUniform(std::forward<ENTRIES>(entries)...);

    glDispatchCompute(numGroups.x(), numGroups.y(), numGroups.z());
    glMemoryBarrier(barriers);

    iloj::gpu::Program::unbind();
}

template<typename T = Texture2D>
void clear(TargetList<T> targets, const ViewPort &viewPort, const Clear::Context &clearContext)
{
    auto plan = [&]() {
        viewPort.apply();
        clearContext.apply(targets, viewPort);
    };

    if (!clearContext.hasClearDepth())
    {
        glDepthMask(GL_FALSE);
    }

    switch (targets.size())
    {
        case 0:
            plan();
            break;
        case 1:
            Context::getInstance().execute(targets.begin()->get(), plan);
            break;
        default:
            Context::getInstance().execute(targets, plan);
    }

    if (!clearContext.hasClearDepth())
    {
        glDepthMask(GL_TRUE);
    }
}
} // namespace iloj::gpu
