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

#include "options.h"

// NOLINTNEXTLINE
#define GPU_GL_BUILD_SHADER(BODY)                                                                                      \
    (std::string{GPU_GL_SHADER_HEADER_VERSION} + std::string{GPU_GL_SHADER_HEADER_EXTENSION} +                         \
     std::string{GPU_GL_SHADER_HEADER_PRECISION} + (BODY))                                                             \
        .c_str()
#define GPU_GL_BUILD_SHADER_WITH_EXTENSION(BODY, EXTENSION)                                                            \
    (std::string{GPU_GL_SHADER_HEADER_VERSION} + std::string{GPU_GL_SHADER_HEADER_EXTENSION} + (EXTENSION) +           \
     std::string{GPU_GL_SHADER_HEADER_PRECISION} + (BODY))                                                             \
        .c_str()
