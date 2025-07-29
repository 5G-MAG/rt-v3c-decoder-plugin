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

#include "VPCCRenderer.h"

#pragma region OpenGL Utils
void LogError(const char *section_name)
{
    GLenum err;
    glFlush();
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        LOG_ERROR("GL Error ", err, " in ", section_name);
    }
    glFlush();
}


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
                     unsigned wrap)
{
    // https://arm-software.github.io/opengl-es-sdk-for-android/compute_intro.html -> 2nd part of the Note in the
    // "Binding a shader image" section:
    //      In OpenGL ES implementations for Android, you need an immutable texture declaration
    //      (i.e. texture generated with glTexStorage2D rather than glTexImage2D) to be able to use those textures as
    //      Image2D in a compute shader without error (or worst, no error, but nothing happening)
    //
    //      Now, iloj uses glTexImage2D when constructing a Texture2D (cf Texture2D::reshape() called by any constructor
    //      with more than 1 param), so here we bypass that behaviour to make Image2D work in compute shaders on
    //      Android.
    //
    //      That also mean we cannot resize the textures in openglES since they are immutable, so we will need to create
    //      a new one if the size need to change
    //
    // More on that: https://community.khronos.org/t/glteximage2d-vs-gltexstorage2d/107695

    // This just provides us with an id
    unsigned id = 0;
    glGenTextures(1, &id);
    LogError("Debug Compute Tex Create");

    // This init the members of the Texture2D class (needed for the image creation), but we have an id so no
    // glTexImage2D is called.  It won't actually setup the values in the texture though (no OpenGL callbacks), so we do
    // that later #ilojhack
    tex = Texture2D(id, w, h, internal_format, interpolation, wrap);
    LogError("Debug Compute Tex Iloj Setup");

    // The actual texture setup, direclty in Opengl
    tex.bind();
    LogError("Debug Compute Tex GL Bind");
    glTexStorage2D(GL_TEXTURE_2D, 1, internal_format, w, h);
    LogError("Debug Compute TexStorage2D");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation);
    LogError("Debug Compute Tex GL Filter");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    LogError("Debug Compute Tex GL Wrap");
    tex.unbind();
    LogError("Debug Compute Tex GL Setup");
}
#pragma endregion

VPCCRenderer::VPCCRenderer(bool render_shadow, std::string name, bool count_points): m_renderShadow(render_shadow){
    
    LOG_INFO("VPCC Renderer Created: ", name);
}


void VPCCRenderer::formatMetadata(const std::unique_ptr<VpccMetadata>& metadata,
    const unsigned int tex_width,
    const unsigned int tex_height)
{
    size_t size = metadata->blockToPatch.size();
    m_metaWidth = tex_width / 16;
    m_metaHeight = tex_height / 16;

    if (size != m_metaWidth * m_metaHeight)
    {
        LOG_ERROR("VPCC Metadata size disrepencies, expected size is ",
            m_metaWidth,
            " x ",
            m_metaHeight,
            " = ",
            m_metaWidth * m_metaHeight,
            " but we have ",
            size,
            " blocks ");
    }

    if (m_bufferUV0UV1.size() == 0 || m_bufferD1NOP.size() == 0)
    {
        m_bufferUV0UV1 = std::vector<float>(size * 4);
        m_bufferD1NOP = std::vector<float>(size * 4);
    }
    if (m_bufferUV0UV1.size() < size * 4) // Resize only if too small to avoid reallocating every time.
    {
        m_bufferUV0UV1.resize(size * 4);
        m_bufferD1NOP.resize(size * 4);     
    }

    auto& btp = metadata->blockToPatch;
    auto& patchBlock = metadata->patchBlockBuffers;

    for (size_t i = 0; i < size; i++)
    {
        int patch = (btp[i] - 1);

        uint16_t U0, V0, U1, V1, D1;
        uint8_t norm, orient, proj;

        if (patch >= 0)
        {
            U0 = patchBlock[patch].U0;
            V0 = patchBlock[patch].V0;
            U1 = patchBlock[patch].U1;
            V1 = patchBlock[patch].V1;
            D1 = patchBlock[patch].D1;
            norm = patchBlock[patch].NormalAxis;
            orient = patchBlock[patch].PatchOrientation;
            proj = patchBlock[patch].ProjectionMode;

            m_bufferUV0UV1[i * 4] = U0;
            m_bufferUV0UV1[i * 4 + 1] = V0;
            m_bufferUV0UV1[i * 4 + 2] = U1;
            m_bufferUV0UV1[i * 4 + 3] = V1;

            m_bufferD1NOP[i * 4] = D1;
            m_bufferD1NOP[i * 4 + 1] = norm;
            m_bufferD1NOP[i * 4 + 2] = orient;
            m_bufferD1NOP[i * 4 + 3] = proj;

        }
        else
        {
            m_bufferUV0UV1[i * 4] = 0.0f;
            m_bufferUV0UV1[i * 4 + 1] = 0.0f;
            m_bufferUV0UV1[i * 4 + 2] = 0.0f;
            m_bufferUV0UV1[i * 4 + 3] = 0.0f;

            m_bufferD1NOP[i * 4] = 0.0f;
            m_bufferD1NOP[i * 4 + 1] = 0.0f;
            m_bufferD1NOP[i * 4 + 2] = 0.0f;
            m_bufferD1NOP[i * 4 + 3] = 0.0f;
        }
    }

    m_texUV0UV1.setContent(m_metaWidth, m_metaHeight, GL_RGBA32F, m_bufferUV0UV1.data(), GL_NEAREST, GL_CLAMP_TO_EDGE);
    m_texD1NOP.setContent(m_metaWidth, m_metaHeight, GL_RGBA32F, m_bufferD1NOP.data(), GL_NEAREST, GL_CLAMP_TO_EDGE);
}


void VPCCRenderer::exportMetadata(const std::unique_ptr<VpccMetadata>& metadata)
{
    int num_patch = metadata->patchBlockBuffers.size();
    
    for (int pid = 0; pid < num_patch; pid++) {

        auto& patch = metadata->patchBlockBuffers[pid];

        LOG_INFO("P", pid, "/", num_patch,
            ": 2D = (", patch.U0, ",", patch.V0,")* (",0, ",", 0,
            ") 3D(", patch.U1, ",", patch.V1, ",", patch.D1, ")* (", 0, ",", 0,
            ",) A = (", patch.NormalAxis, ",", 0, ",", 0,
            ") Or =", patch.PatchOrientation, " P = ", patch.ProjectionMode, " = > ", 0, "AxisOfAdditionalPlane = 0");
    }
}



void VPCCRenderer::enableRenderOptions() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#ifdef _WIN64
    glEnable(GL_PROGRAM_POINT_SIZE);
#endif
}

void VPCCRenderer::disableRenderOptions() {
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDisable(GL_BLEND);
#ifdef _WIN64
    glDisable(GL_PROGRAM_POINT_SIZE);
#endif
}