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

#if defined(__CYGWIN32__)
#define INTERFACE_API __stdcall
#define INTERFACE_EXPORT __declspec(dllexport)
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(WINAPI_FAMILY)
#define INTERFACE_API __stdcall
#define INTERFACE_EXPORT __declspec(dllexport)
#elif defined(__MACH__) || defined(__ANDROID__) || defined(__linux__) || defined(LUMIN)
#define INTERFACE_API
#define INTERFACE_EXPORT __attribute__((visibility("default")))
#else
#define INTERFACE_API
#define INTERFACE_EXPORT
#endif


#include <iloj/gpu/texture.h>

using HANDLE = void *;


struct VpccPatchMetadata
{
    // public:
    //uint16_t frame_index{0};
    uint16_t U0{0};
    uint16_t V0{0};
    uint16_t U1{0};
    uint16_t V1{0};
    uint16_t D1{0};
    uint16_t NormalAxis{0};
    uint16_t PatchOrientation{0};
    uint16_t ProjectionMode{0};

    // VpccPatchMetadata(){};
    // VpccPatchMetadata(const VpccPatchMetadata &) = delete;
};

struct VpccMetadata
{
    int frame_index{-1};
    int frame_width, frame_height;
    //pcc::V3CParameterSet pccVps;
    std::vector<VpccPatchMetadata> patchBlockBuffers = {};
    std::vector<size_t> blockToPatch = {};
};

struct GenericMetadata
{
    enum ContentType
    {
        Unknown = 0,
        MIV,
        VPCC
    };

    void* unused;
    std::unique_ptr<VpccMetadata> VPCCMetadata;

    int contentId{-1};
    int segmentId{-1};

    ContentType contentType{ContentType::Unknown};
};


using Metadata = GenericMetadata;

struct TextureProperty
{
    HANDLE handle{};
    unsigned width{};
    unsigned height{};
    unsigned format{};

    auto toRegularTexture(unsigned interpolation, unsigned wrap) const -> iloj::gpu::Texture2D
    {
        return {static_cast<int>(reinterpret_cast<std::size_t>(handle)), width, height, format, interpolation, wrap};
    }
};

struct Viewport
{
    unsigned width{};
    unsigned height{};
    unsigned left{};
    unsigned bottom{};

    auto toRegularViewport() const -> iloj::gpu::ViewPort { return {width, height, left, bottom}; }
};

enum FlipMode
{
    None = 0,
    Horizontal,
    Vertical,
    All
};

struct JobVPCC
{

    Viewport viewport{};
    iloj::math::Mat4x4f m_model;
    iloj::math::Mat4x4f m_view;
    iloj::math::Mat4x4f m_proj;
    float m_pointSize;

    float m_shadowAlpha;
    float m_shadowOffset;        
};

using JobList = std::vector<JobVPCC>;

using OnCreateCallback = void(const char *configFile);
using OnRenderCallback = void(const Metadata *metaData,
                              const TextureProperty *occupancyMap,
                              const TextureProperty *geometryMap,
                              const TextureProperty *textureMap,
                              const TextureProperty *transparencyMap,
                              unsigned nbJobs,
                              const JobVPCC *jobList,
                              const TextureProperty *canvas);
using OnReleaseCallback = void();
