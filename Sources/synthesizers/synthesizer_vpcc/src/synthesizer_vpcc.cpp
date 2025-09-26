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

#include <iloj/gpu/options.h>
#include <iloj/misc/json.h>
#include <iloj/misc/logger.h>
#include "synthesizer_vpcc.h"

using namespace iloj::math;
using namespace iloj::gpu;
using namespace iloj::misc;

Synthesizer::Synthesizer(const std::string &configFile, unsigned synthesizerId)
{
    LOG_INFO("Synthesizer::Synthesizer ", configFile);

    // Parsing the JSON
    auto json =
        JSON::Object::fromFile(configFile).getItem<JSON::Array>("RendererList").getItem<JSON::Object>(synthesizerId);

    bool render_shadow = false;
    if (auto &item = json.getItem("RenderShadow"))
    {
        render_shadow = item.as<bool>();
    }

    m_Renderer = std::make_unique<ModelBuilder>(render_shadow, false);
    return;    
}

Synthesizer::~Synthesizer() { m_Renderer.reset(); }

Synthesizer::GLCapabilities::GLCapabilities()
    : maxTextureBufferSize{TextureBuffer::getMaxSize()},
      hasGLFrameBufferFetchEXT{hasExtension("GL_EXT_shader_framebuffer_fetch")},
      hasGLFrameBufferFetchARM{hasExtension("GL_ARM_shader_framebuffer_fetch")},
      defaultBlendingFormat
{
#if GPU_GL_TAINT == GPU_GL_TAINT_ES
    GL_RGBA8 // Some drivers do not support GL_RGBA16
#else
    GL_RGBA16,
#endif
}
{
    LOG_INFO("TextureBuffer::getMaxSize(): ", TextureBuffer::getMaxSize());
    LOG_INFO("hasGLFrameBufferFetchEXT: ", hasGLFrameBufferFetchEXT);
    LOG_INFO("hasGLFrameBufferFetchARM: ", hasGLFrameBufferFetchARM);
    LOG_INFO("defaultBlendingFormat: ", defaultBlendingFormat);
}


void Synthesizer::synthesize(const Metadata &metaData,
                             const iloj::gpu::Texture2D &occupancyMap,
                             const iloj::gpu::Texture2D &geometryMap,
                             const iloj::gpu::Texture2D &textureMap,
                             const iloj::gpu::Texture2D &transparencyMap,
                             iloj::gpu::Texture2D &canvas)
{

    if (m_copyTexture)      { CopyTexture (textureMap,   m_texture);   }
    if (m_copyGeometry)     { CopyTexture (geometryMap,  m_geometry);  }
    if (m_copyOccupancy)    { CopyTexture (occupancyMap, m_occupancy); }

    UpdateIndex(metaData);

    if (m_exportMetadata) {
        m_Renderer->exportMetadata(metaData.VPCCMetadata);
        m_exportMetadata = false;
    }


    m_Renderer->render(metaData, occupancyMap, geometryMap, textureMap, transparencyMap, canvas);
}

auto Synthesizer::getGLCapabilities() -> const GLCapabilities &
{
    static GLCapabilities capabilities;
    return capabilities;
}

void Synthesizer::UpdateIndex(const Metadata &metaData) {
    int fid = metaData.VPCCMetadata->frame_index;
    if (fid < m_frameIndex) {
        m_GOPIndex++;
    }
    m_frameIndex = fid;
}

void Synthesizer::CopyTexture(const Texture2D& source, Texture2D& dest) {
    
    if (source.width() != dest.width() || source.height() != dest.height()) {
        dest.reshape(source.width(), source.height(), source.getInternalFormat(), GL_NEAREST, GL_CLAMP_TO_EDGE);
    }

    image::execute( { dest }, 
                    dest.getViewPort(), 
                    Clear::Context::None(), 
                    Blending::Context::None(), 
                    image::Program::copy(), 
                    Uniform::Entry<Texture2D>("Input", source));
}


void Synthesizer::setIndirectBufferPtr(void *ptr) { m_Renderer->setIndirectBufferPtr(ptr); }
void Synthesizer::setDecimationLevel(int level) { m_Renderer->setDecimationLevel(level); }
void Synthesizer::setMVP(float* MVP) { m_Renderer->setMVP(MVP); }
void Synthesizer::setNumVertexPerPoint(int num_vert_per_point) { m_Renderer->setNumVertexPerPoint(num_vert_per_point); }
void Synthesizer::setPositionTexture(TextureProperty* position){m_Renderer->setPositionTexture(position);}
void Synthesizer::setShadowTexture(TextureProperty* shadow){m_Renderer->setShadowTexture(shadow);}
void Synthesizer::setMaxBbox(float size) { m_Renderer->setMaxBbox(size); }
void Synthesizer::ExportMetadata() { m_exportMetadata = true; }

                                                  