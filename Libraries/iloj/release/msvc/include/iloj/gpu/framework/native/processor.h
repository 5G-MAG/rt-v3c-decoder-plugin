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

#include <iloj/gpu/processor.h>

#if defined _WIN64
#include <windows.h>
#elif defined __ANDROID__
#include <EGL/egl.h>

#elif __APPLE__
    #include <TargetConditionals.h>
    #if TARGET_OS_OSX 
        #define GL_SILENCE_DEPRECATION
        #include <OpenGL/OpenGL.h>
    #elif TARGET_OS_IPHONE
        // iOS, tvOS, or visionOS device
        #include <OpenGLES/EAGL.h>
    #else
    #   error "Unsupported Apple platform"
    #endif

#else
// TODO
#endif

namespace iloj::gpu::framework::native
{
#if defined _WIN64
class Processor: public iloj::gpu::Processor
{
private:
    HINSTANCE m_hInstance = nullptr;
    HWND m_hWnd = nullptr;
    std::array<HDC, 2> m_hDC{};
    std::array<HGLRC, 2> m_wglContext{};
    bool m_restoreOn = false;

public:
    Processor(HGLRC sharedHandle = nullptr, bool restoreOn = false, bool contextOn = true);
    ~Processor() override;
    Processor(const Processor &) = delete;
    Processor(Processor &&) noexcept = default;
    auto operator=(const Processor &) -> Processor & = delete;
    auto operator=(Processor &&) noexcept -> Processor & = default;
    [[nodiscard]] auto getNativeHandle() const -> GLvoid * override { return m_wglContext[0]; }

protected:
    void acquireGLContext() override;
    void releaseGLContext() override;

private:
    void initWindow();
    void closeWindow();
    void initDevice(HGLRC sharedHandle);
    void releaseDevice();
};
#elif defined __ANDROID__
class Processor: public iloj::gpu::Processor
{
private:
    std::array<EGLDisplay, 2> m_display{};
    std::array<EGLContext, 2> m_context{};
    std::array<EGLSurface, 2> m_readSurface{};
    std::array<EGLSurface, 2> m_drawSurface{};
    bool m_restoreOn = false;

public:
    Processor(EGLContext sharedHandle = nullptr, bool restoreOn = false, bool contextOn = true);
    ~Processor() override;
    Processor(const Processor &) = delete;
    Processor(Processor &&) noexcept = default;
    auto operator=(const Processor &) -> Processor & = delete;
    auto operator=(Processor &&) noexcept -> Processor & = default;
    auto getNativeHandle() const -> GLvoid * override { return m_context[0]; }

protected:
    void acquireGLContext() override;
    void releaseGLContext() override;
};
#elif __APPLE__

#if TARGET_OS_OSX 
class Processor: public iloj::gpu::Processor
{
private:
    std::array<CGLContextObj, 2> m_cglContext;
    bool m_restoreOn = false;

public:
    Processor(CGLContextObj sharedHandle = nullptr, bool restoreOn = false, bool contextOn = true);
    ~Processor() override;
    Processor(const Processor &) = delete;
    Processor(Processor &&) noexcept = default;
    auto operator=(const Processor &) -> Processor & = delete;
    auto operator=(Processor &&) noexcept -> Processor & = default;
    auto getNativeHandle() const -> GLvoid * override { return m_cglContext[0]; }

protected:
    void acquireGLContext() override;
    void releaseGLContext() override;
};

#elif TARGET_OS_IPHONE

class Processor: public iloj::gpu::Processor
{
private:
    std::array<EGLDisplay, 2> m_display{};
    std::array<EGLContext, 2> m_context{};
    std::array<EGLSurface, 2> m_readSurface{};
    std::array<EGLSurface, 2> m_drawSurface{};
    bool m_restoreOn = false;

public:
    Processor(EGLContext sharedHandle = nullptr, bool restoreOn = false, bool contextOn = true);
    ~Processor() override;
    Processor(const Processor &) = delete;
    Processor(Processor &&) noexcept = default;
    auto operator=(const Processor &) -> Processor & = delete;
    auto operator=(Processor &&) noexcept -> Processor & = default;
    auto getNativeHandle() const -> GLvoid * override { return m_context[0]; }

protected:
    void acquireGLContext() override;
    void releaseGLContext() override;
};

#endif // TARGET_OS_...

#endif

} // namespace iloj::gpu::framework::native
