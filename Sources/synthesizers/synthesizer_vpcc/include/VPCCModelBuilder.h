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

#include <fstream>
#include "VPCCRenderer.h"

using namespace iloj::gpu;
using namespace iloj::misc;

class VPCCModelBuilder: public VPCCRenderer
{
public:
    VPCCModelBuilder(bool render_shadow, bool count_points);

    void render(const Metadata &metaData,
                const iloj::gpu::Texture2D &occupancyMap,
                const iloj::gpu::Texture2D &geometryMap,
                const iloj::gpu::Texture2D &textureMap,
                const iloj::gpu::Texture2D &transparencyMap,                
                iloj::gpu::Texture2D &canvas);

    void setIndirectBufferPtr(void *ptr) override {m_indirectBuffer = static_cast<GLuint>(reinterpret_cast<std::size_t>(ptr));}
    void setMVP(float* MVP) override { for (int i = 0; i < 16; i++) { m_MVP[i] = MVP[i]; }}
    void setDecimationLevel(int level) override { m_decimationLevel = level;}
    void setNumVertexPerPoint(int num_vert_per_point) override { m_numVertPerPoint = num_vert_per_point;}

private:
    void printComputeCapabilities();
    std::string LoadShader(const std::string path);
    void create_model_builder_program();    
    void create_decimation_program();
     
    
    void build_model(const Metadata& metaData,
        const iloj::gpu::Texture2D& occupancyMap,
        const iloj::gpu::Texture2D& geometryMap,
        const iloj::gpu::Texture2D& textureMap,
        iloj::gpu::Texture2D& canvas);

private:
    bool m_isTexInit = false;

    //Size used for texture initialization
    int m_widthInit = -1;
    int m_heightInit = -1;

    int m_lastFrameId = -1;

    // Shadow Rendering
    Program m_clear_image_program;
    Program m_model_builder_program;
    Program m_model_builder_program_dd;
    Program m_compute_decimation_program;

    //std::unique_ptr<Texture2D> m_decimationTex;
    Texture2D m_decimationTex;

    std::unique_ptr<ImageES> m_posImg, m_shadowImg, m_colImg, m_decImg;

    GLuint m_atomicsBuffer = 0;
    GLuint m_indirectBuffer = 0;

    int m_decimationLevel = 1;
    int m_numVertPerPoint = 1;

    iloj::math::Mat4x4f m_MVP{};
};
