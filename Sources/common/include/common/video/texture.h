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

#include <common/misc/types.h>
#include <iloj/gpu/definitions.h>

namespace SharedTexture2D
{
class Base
{
public:
    Base() = default;
    virtual ~Base(){};
    Base(const Base &) = delete;
    Base(Base &&other) = default;
    auto operator=(const Base &) -> Base & = delete;
    auto operator=(Base &&other) -> Base & = default;
    virtual void lock() = 0;
    virtual auto getHandle() -> HANDLE = 0;
    virtual auto getGLName() const -> unsigned = 0;
    virtual void unlock() = 0;
};

#ifdef _WIN64
class D3D11: public Base
{
private:
    HANDLE m_d3dDevice{};
    HANDLE m_d3dName{};
    unsigned m_glName{};
    HANDLE m_handleD3D{};
    HANDLE m_handle{};

public:
    D3D11(HANDLE d3dDevice, HANDLE d3dName);
    ~D3D11() override;
    D3D11(const D3D11 &) = delete;
    D3D11(D3D11 &&other);
    auto operator=(const D3D11 &) -> D3D11 & = delete;
    auto operator=(D3D11 &&other) -> D3D11 &;
    void lock() override;
    auto getHandle() -> HANDLE override { return m_d3dName; }
    auto getGLName() const -> unsigned override { return m_glName; }
    void unlock() override;
};
#endif

class OpenGL: public Base
{
private:
    unsigned m_glName{};

public:
    OpenGL(unsigned glName);
    ~OpenGL() override {}
    OpenGL(const OpenGL &) = delete;
    OpenGL(OpenGL &&other);
    auto operator=(const OpenGL &) -> OpenGL & = delete;
    auto operator=(OpenGL &&other) -> OpenGL &;
    void lock() override {}
    auto getHandle() -> HANDLE override { return reinterpret_cast<HANDLE>(static_cast<std::size_t>(m_glName)); }
    auto getGLName() const -> unsigned override { return m_glName; }
    void unlock() override {}
};

} // namespace SharedTexture2D
