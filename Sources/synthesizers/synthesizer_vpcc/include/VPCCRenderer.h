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

#include <iloj/gpu/image.h>
#include <iloj/gpu/functions.h>
#include <iloj/misc/logger.h>

using namespace iloj::gpu;
using namespace iloj::misc;

#pragma region OpenGL Utils
void LogError(const char *section_name);

/// <summary>
/// Init an immutable iloj texture 2D (not possible in vanilla iloj)
/// Needed in Android GLES as a basis for writable images in compute shaders 
/// </summary>
/// <param name="tex">Texture to init</param>
/// <param name="w">Texture width</param>
/// <param name="h">Texture height</param>
/// <param name="internal_format">Texture pixel format (ex GL_RGBA32F)</param>
/// <param name="interpolation">Texture interpolation mode (ex GL_NEAREST)</param>
/// <param name="wrap">Texture wrap mode (ex: GL_CLAMP_TO_EDGE)</param>
void initTextureGLES(Texture2D &tex,
                     unsigned w,
                     unsigned h,
                     unsigned internal_format,
                     unsigned interpolation,
                     unsigned wrap);
#pragma endregion


/// <summary>
/// Base abstract class for VPCC Renderer
/// Implement FBO init and metadata management, but no rendering (to be done in children)
/// </summary>
class VPCCRenderer
{
public:
    struct DebugOptions
    {
        bool m_debugTexture = false;
        bool m_debugGeometry = false;
        bool m_debugOccupancy = false;
    };

    DebugOptions m_options;

public:
    VPCCRenderer(bool render_shadow, std::string name, bool count_points = false);
    virtual ~VPCCRenderer(){};

    //abstract, to implement
    virtual void render(const Metadata &metaData,
                        const iloj::gpu::Texture2D &occupancyMap,
                        const iloj::gpu::Texture2D &geometryMap,
                        const iloj::gpu::Texture2D &textureMap,
                        const iloj::gpu::Texture2D &transparencyMap,                        
                        iloj::gpu::Texture2D &canvas) = 0;    

    virtual void setIndirectBufferPtr(void *ptr){};
    virtual void setDecimationLevel(int level){};
    virtual void setMVP(float* MVP){};
    virtual void setNumVertexPerPoint(int num_vert_per_point){};
    void setPositionTexture(TextureProperty *position)
    {
        m_posTex = position->toRegularTexture(GL_NEAREST, GL_CLAMP_TO_EDGE);
    }
    void setShadowTexture(TextureProperty *shadow){
        m_shadowTex = shadow->toRegularTexture(GL_NEAREST, GL_CLAMP_TO_EDGE);
    }
    void setMaxBbox(float size) {
        m_maxBbox = size;
    }

    void exportMetadata(const std::unique_ptr<VpccMetadata>& metadata);

    void setGlobal3DPatchOffsets(float N_off, float T_off, float B_off) {
        m_NOff = N_off;
        m_TOff = T_off;
        m_BOff = B_off;
    }

    void setDynamicDecimation(bool useDD) {
        m_useDD = useDD;
    }

    void setDecimationRanges(float r1, float r2, float vp_cull_factor) {
        m_r1 = r1;
        m_r2 = r2;
        m_vp_cull_factor = vp_cull_factor;
    }

    void setForceRebuild(bool force_rebuild) {
        m_forceRebuild = force_rebuild;
    }

protected:
    //Metadata formatting
    void formatMetadata(const std::unique_ptr<VpccMetadata>& metaData, const unsigned int tex_width, const unsigned int tex_height);
    //Call all the glEnable stuff
    void enableRenderOptions();
    //Deactivate all the glEnable stuff
    void disableRenderOptions();

protected:

    bool m_renderShadow{false};       
    Texture2D m_shadowTex;
    Texture2D m_posTex;

    //float m_maxBbox = 1024.0f;
    float m_maxBbox = 2048.0f;
    ////Metadata

    //Sizes
    size_t m_metaWidth{0};
    size_t m_metaHeight{0};

    //Intermediate buffers
    std::vector<float> m_bufferUV0UV1{};
    std::vector<float> m_bufferD1NOP{};

    //Textures
    Texture2D m_texUV0UV1;
    Texture2D m_texD1NOP;

    float m_NOff{ 0 };
    float m_BOff{ 0 };
    float m_TOff{ 0 };

    float m_r1 = 20.0f;
    float m_r2 = 50.0f;
    float m_vp_cull_factor = 1.5f;

    bool m_forceRebuild = false;
    bool m_useDD = false;
};


