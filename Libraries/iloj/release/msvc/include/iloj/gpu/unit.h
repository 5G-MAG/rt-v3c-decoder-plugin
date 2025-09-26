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

#include "context.h"
#include <memory>

namespace iloj::gpu
{
class Unit
{
protected:
    std::unique_ptr<Context> m_context;

public:
    Unit() = default;
    virtual ~Unit() = default;
    Unit(const Unit &) = delete;
    Unit(Unit &&) noexcept = default;
    auto operator=(const Unit &) -> Unit & = delete;
    auto operator=(Unit &&) noexcept -> Unit & = default;
    auto getInternalContext() -> Context & { return *m_context; }
    [[nodiscard]] virtual auto getNativeHandle() const -> GLvoid * = 0;

protected:
    virtual void acquireGLContext() {}
    virtual void releaseGLContext() {}

protected:
    void setupContext();
    void clearContext();
};
} // namespace iloj::gpu
