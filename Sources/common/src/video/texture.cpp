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

#include <iloj/gpu/functions.h>
#include <common/video/texture.h>

using namespace iloj::gpu;

namespace SharedTexture2D
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN64
D3D11::D3D11(HANDLE d3dDevice, HANDLE d3dName): m_d3dDevice{d3dDevice}, m_d3dName{d3dName}
{
    // Create GL texture
    glGenTextures(1, &m_glName);

    // Register D3D11 device with GL
    m_handleD3D = wglDXOpenDeviceNV(m_d3dDevice);

    // Register D3D11 texture with GL
    m_handle = wglDXRegisterObjectNV(m_handleD3D, d3dName, m_glName, GL_TEXTURE_2D, WGL_ACCESS_READ_WRITE_NV);
}

D3D11::~D3D11()
{
    // Unregister D3D11 texture with GL
    wglDXUnregisterObjectNV(m_handleD3D, m_handle);

    // Unregister D3D11 device with GL
    wglDXCloseDeviceNV(m_handleD3D);

    // Delete GL texture
    glDeleteTextures(1, &m_glName);
}

D3D11::D3D11(D3D11 &&other)
{
    m_d3dDevice = other.m_d3dDevice;
    m_handleD3D = other.m_handleD3D;
    m_glName = other.m_glName;
    m_handle = other.m_handle;

    other.m_d3dDevice = {};
    other.m_handleD3D = {};
    other.m_glName = {};
    other.m_handle = {};
}

auto D3D11::operator=(D3D11 &&other) -> D3D11 &
{
    m_d3dDevice = other.m_d3dDevice;
    m_handleD3D = other.m_handleD3D;
    m_glName = other.m_glName;
    m_handle = other.m_handle;

    other.m_d3dDevice = {};
    other.m_handleD3D = {};
    other.m_glName = {};
    other.m_handle = {};

    return *this;
}

void D3D11::lock() { wglDXLockObjectsNV(m_handleD3D, 1, &m_handle); }

void D3D11::unlock() { wglDXUnlockObjectsNV(m_handleD3D, 1, &m_handle); }
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
OpenGL::OpenGL(GLuint glName): m_glName{glName} {}

OpenGL::OpenGL(OpenGL &&other)
{
    m_glName = other.m_glName;
    other.m_glName = {};
}

auto OpenGL::operator=(OpenGL &&other) -> OpenGL &
{
    m_glName = other.m_glName;
    other.m_glName = {};

    return *this;
}

} // namespace SharedTexture2D
