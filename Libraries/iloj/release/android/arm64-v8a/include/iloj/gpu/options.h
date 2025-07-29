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

#include <iloj/misc/macros.h>

#define GPU_GL_TAINT_CORE 1
#define GPU_GL_TAINT_ES 2

#if defined __ANDROID__
// NOLINTNEXTLINE
#define GPU_GL_MAJOR 3
// NOLINTNEXTLINE
#define GPU_GL_MINOR 2
// NOLINTNEXTLINE
#define GPU_GL_TAINT GPU_GL_TAINT_ES
// NOLINTNEXTLINE
#define GPU_GL_SHADER_HEADER_VERSION "#version 320 es\n"
#define GPU_GL_SHADER_HEADER_EXTENSION                                                                                 \
    "#extension GL_EXT_geometry_shader : enable\n"                                                                     \
    "#extension GL_EXT_tessellation_shader : enable\n"                                                                 \
    "#extension GL_OES_EGL_image_external_essl3 : enable\n"                                                            \
    "#extension GL_EXT_YUV_target : enable\n"

#define GPU_GL_SHADER_HEADER_PRECISION                                                                                 \
    "precision highp float;\n"                                                                                         \
    "precision highp sampler2D;\n"                                                                                     \
    "precision highp isampler2D;\n"                                                                                    \
    "precision highp usampler2D;\n"                                                                                    \
    "precision highp samplerBuffer;\n"                                                                                 \
    "precision highp isamplerBuffer;\n"                                                                                \
    "precision highp usamplerBuffer;\n"

#elif defined __APPLE__

#include <TargetConditionals.h>

#if TARGET_OS_OSX
// NOLINTNEXTLINE
#define GPU_GL_MAJOR 4
// NOLINTNEXTLINE
#define GPU_GL_MINOR 1
// NOLINTNEXTLINE
#define GPU_GL_TAINT GPU_GL_TAINT_CORE
// NOLINTNEXTLINE
#define GPU_GL_SHADER_HEADER_VERSION "#version 410 core\n"
#define GPU_GL_SHADER_HEADER_EXTENSION ""
#define GPU_GL_SHADER_HEADER_PRECISION ""

#else
// NOLINTNEXTLINE
#define GPU_GL_MAJOR 3
// NOLINTNEXTLINE
#define GPU_GL_MINOR 0
// NOLINTNEXTLINE
#define GPU_GL_TAINT GPU_GL_TAINT_ES
// NOLINTNEXTLINE
#define GPU_GL_SHADER_HEADER_VERSION "#version 300 es\n"
#define GPU_GL_SHADER_HEADER_EXTENSION ""
#define GPU_GL_SHADER_HEADER_PRECISION ""

#endif

#else
// NOLINTNEXTLINE
#define GPU_GL_MAJOR 4
// NOLINTNEXTLINE
#define GPU_GL_MINOR 5
// NOLINTNEXTLINE
#define GPU_GL_TAINT GPU_GL_TAINT_CORE
// NOLINTNEXTLINE
#define GPU_GL_SHADER_HEADER_VERSION "#version 450 core\n"
#define GPU_GL_SHADER_HEADER_EXTENSION ""
#define GPU_GL_SHADER_HEADER_PRECISION ""

#endif

// NOLINTNEXTLINE
#define GPU_GL_VERSION CONCAT(GPU_GL_MAJOR, CONCAT(_, GPU_GL_MINOR))
