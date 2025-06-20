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
#include <iloj/gpu/definitions.h>
#include <iloj/misc/logger.h>

using namespace iloj::gpu;

namespace TextureFormat
{
enum UnityTextureFormat
{
    None = 0,
    R8 = 63,
    R16 = 9,
    Rhalf = 15,
    RFloat = 18,
    RGB24 = 3,
    RGB48 = 73,
    RGBA32 = 4,
    RGBA64 = 74,
    RGBAHalf = 17,
    RGBAFloat = 20,
};

auto getGLFromUnityTextureFormat(unsigned fmt) -> unsigned
{
    switch (fmt)
    {
        case UnityTextureFormat::None:
            return GL_NONE;
        case UnityTextureFormat::R8:
            return GL_R8;
        case UnityTextureFormat::R16:
            return GL_R16;
        case UnityTextureFormat::Rhalf:
            return GL_R16F;
        case UnityTextureFormat::RFloat:
            return GL_R32F;
        case UnityTextureFormat::RGB24:
            return GL_RGB8;
        case UnityTextureFormat::RGB48:
            return GL_RGB16;
        case UnityTextureFormat::RGBA32:
            return GL_RGBA8;
        case UnityTextureFormat::RGBA64:
            return GL_RGBA16;
        case UnityTextureFormat::RGBAHalf:
            return GL_RGBA16F;
        case UnityTextureFormat::RGBAFloat:
            return GL_RGBA32F;
        default:
            LOG_ERROR("Unsupported pixel format");
            return GL_NONE;
    }
}

auto getUnityFromGLTextureFormat(int fmt) -> unsigned
{
    switch (fmt)
    {
        case GL_NONE:
            return UnityTextureFormat::None;
        case GL_R8:
            return UnityTextureFormat::R8;
        case GL_R16:
            return UnityTextureFormat::R16;
        case GL_R16F:
            return UnityTextureFormat::Rhalf;
        case GL_R32F:
            return UnityTextureFormat::RFloat;
        case GL_RGB8:
            return UnityTextureFormat::RGB24;
        case GL_RGB16:
            return UnityTextureFormat::RGB48;
        case GL_RGBA8:
            return UnityTextureFormat::RGBA32;
        case GL_RGBA16:
            return UnityTextureFormat::RGBA64;
        case GL_RGBA16F:
            return UnityTextureFormat::RGBAHalf;
        case GL_RGBA32F:
            return UnityTextureFormat::RGBAFloat;
        default:
            LOG_ERROR("Unsupported pixel format: ", fmt);
            return UnityTextureFormat::None;
    }
}

}