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

#include "types_vpcc.h"
#include "VPCCModelBuilder.h"

using namespace iloj::gpu;


// Edit to change renderers
//typedef VPCCRendererComputeIndirect ShadowRenderer;
//typedef VPCCRendererComputeIndirect NoShadowRenderer;
//typedef VPCCRendererDebug           DebugRenderer;

typedef VPCCModelBuilder            ModelBuilder;
//typedef VPCCDynamicDecimationBuilder  ModelBuilder;


class Synthesizer
{
private:
    struct GLCapabilities
    {
    public:
        int maxTextureBufferSize{};
        bool hasGLFrameBufferFetchEXT{};
        bool hasGLFrameBufferFetchARM{};
        unsigned defaultBlendingFormat{};

    public:
        GLCapabilities();
        ~GLCapabilities() = default;
        GLCapabilities(const GLCapabilities &) = default;
        GLCapabilities(GLCapabilities &&) noexcept = default;
        auto operator=(const GLCapabilities &) -> GLCapabilities & = delete;
        auto operator=(GLCapabilities &&) noexcept -> GLCapabilities & = default;
    };

private:

    std::unique_ptr<VPCCRenderer> m_Renderer; 

public:
    Synthesizer() = default;
    ~Synthesizer();
    Synthesizer(const std::string &configFile, unsigned synthesizerId);
    Synthesizer(const Synthesizer &) = delete;
    Synthesizer(Synthesizer &&) noexcept = default;
    auto operator=(const Synthesizer &) -> Synthesizer & = delete;
    auto operator=(Synthesizer &&) noexcept -> Synthesizer & = default;

    void synthesize(const Metadata &metaData,
                    const iloj::gpu::Texture2D &occupancyMap,
                    const iloj::gpu::Texture2D &geometryMap,
                    const iloj::gpu::Texture2D &textureMap,
                    const iloj::gpu::Texture2D &transparencyMap,                    
                    iloj::gpu::Texture2D &canvas);

    void setIndirectBufferPtr(void *ptr);
    void setDecimationLevel(int level);
    void setMVP(float* MVP);
    void setNumVertexPerPoint(int num_vert_per_point);
    void setPositionTexture(TextureProperty *position);
    void setShadowTexture(TextureProperty * shadow);
    void setMaxBbox(float size);

    void setTextureTexture(TextureProperty* texture)        { m_copyTexture = true;     m_texture   =   texture->toRegularTexture(GL_NEAREST, GL_CLAMP_TO_EDGE); }
    void setGeometryTexture(TextureProperty* geometry)      { m_copyGeometry = true;    m_geometry  =  geometry->toRegularTexture(GL_NEAREST, GL_CLAMP_TO_EDGE); }
    void setOccupancyTexture(TextureProperty* occupancy)    { m_copyOccupancy = true;   m_occupancy = occupancy->toRegularTexture(GL_NEAREST, GL_CLAMP_TO_EDGE); }

    static auto getGLCapabilities() -> const GLCapabilities &;

    void UpdateIndex(const Metadata& metaData);

    void CopyTexture(const Texture2D& source, Texture2D& dest);

    void ExportMetadata();

    void setGlobal3DPatchOffsets(float N_off, float T_off, float B_off) {
        m_Renderer->setGlobal3DPatchOffsets(N_off, T_off, B_off);
    }

    void setDynamicDecimation(bool useDD) {
        m_Renderer->setDynamicDecimation(useDD);
    }

    void setDecimationRanges(float r1, float r2, float vp_cull_factor) {
        m_Renderer->setDecimationRanges(r1,r2, vp_cull_factor);
    }

    void setForceRebuild(bool force_rebuild) {
        m_Renderer->setForceRebuild(force_rebuild);
    }

private:

    Texture2D m_texture;
    Texture2D m_geometry;
    Texture2D m_occupancy;

    bool m_copyTexture=false;
    bool m_copyGeometry=false;
    bool m_copyOccupancy=false;
    bool m_exportMetadata = false;

public: 
    int m_frameIndex{ 0 };
    int m_GOPIndex{ 0 };

};    


