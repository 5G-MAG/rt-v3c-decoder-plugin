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

#include <iloj/misc/memory.h>

namespace iloj::gpu
{
class Renderer;

class Delegate
{
    friend class Renderer;

private:
    iloj::misc::Reference<Renderer> m_renderer;

public:
    Delegate() = default;
    virtual ~Delegate() = default;
    Delegate(const Delegate &) = default;
    Delegate(Delegate &&) noexcept = default;
    auto operator=(const Delegate &) -> Delegate & = default;
    auto operator=(Delegate &&) noexcept -> Delegate & = default;
    auto getRenderer() -> Renderer & { return m_renderer.get(); }

protected:
    void attach(Renderer &renderer) { m_renderer = renderer; }
    virtual void bind() {}
    virtual void populate() {}
    virtual void update() {}
    virtual void reshape(int /*unused*/, int /*unused*/) {}
};
} // namespace iloj::gpu
