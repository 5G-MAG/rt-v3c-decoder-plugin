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

#include <iloj/gpu/processor.h>

namespace iloj::gpu::framework::lazy
{
class Processor: public iloj::gpu::Processor
{
private:
    bool m_contextOn = true;

public:
    Processor(bool contextOn);
    ~Processor() override;
    Processor(const Processor &) = delete;
    Processor(Processor &&) noexcept = default;
    auto operator=(const Processor &) -> Processor & = delete;
    auto operator=(Processor &&) noexcept -> Processor & = default;
    auto getNativeHandle() const -> GLvoid * override { return nullptr; }
    void acquireGLContext() override {}
    void releaseGLContext() override {}
};

} // namespace iloj::gpu::framework::lazy
