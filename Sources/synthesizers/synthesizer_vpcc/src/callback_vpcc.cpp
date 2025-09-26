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

#include <fstream>
#include <iloj/gpu/framework/lazy/processor.h>
#include <iloj/misc/filesystem.h>
#include <iloj/misc/logger.h>
#include <synthesizer_vpcc.h>
#include <types_vpcc.h>
#include <common/texture_format.h>

static std::ofstream g_logStream_pcc;
static std::unique_ptr<iloj::gpu::framework::lazy::Processor> g_processor_pcc;
static std::unique_ptr<Synthesizer> g_synthesizer_pcc;

extern "C" INTERFACE_EXPORT void INTERFACE_API OnCreateEvent(const char *configFile, unsigned synthesizerId)
{
    using namespace iloj::misc;

    g_logStream_pcc.open(FileSystem::Path{configFile}.getParent().toString() +
                     "/V3CImmersiveSynthesizerVPCC.log");

    Logger::getInstance().setStream(g_logStream_pcc);
    Logger::getInstance().setLevel(Logger::Level::Info);
    Logger::getInstance().enableFlushOnAppend(true);

    if (!g_processor_pcc)
    {
        g_processor_pcc = std::make_unique<iloj::gpu::framework::lazy::Processor>(true);
    }

    g_processor_pcc->execute([&] {
    
    g_synthesizer_pcc = std::make_unique<Synthesizer>(configFile, synthesizerId); 
    
    });

    LOG_INFO("OnCreateEvent");
}

extern "C" INTERFACE_EXPORT bool INTERFACE_API OnCapabilityEvent(const Metadata *metaData)
{
    return (metaData!=nullptr && metaData->VPCCMetadata->patchBlockBuffers.size() > 0);
}

extern "C" INTERFACE_EXPORT void INTERFACE_API OnRenderEvent(const Metadata *metaData,
                                                             const TextureProperty *occupancyMap,
                                                             const TextureProperty *geometryMap,
                                                             const TextureProperty *textureMap,
                                                             const TextureProperty *transparencyMap,
                                                             unsigned nbJobs,
                                                             const void *unused,
                                                             const TextureProperty *canvas)
{
    using namespace iloj::gpu;

    if (g_processor_pcc && g_synthesizer_pcc)
    {
        g_processor_pcc->execute([&]() {

            static Metadata defaultMetadata;
            auto regularCanvas = canvas->toRegularTexture(GL_LINEAR, GL_CLAMP_TO_EDGE);

            g_synthesizer_pcc->synthesize(metaData ? *metaData : defaultMetadata,
                                          occupancyMap->toRegularTexture(GL_NEAREST, GL_CLAMP_TO_EDGE),
                                          geometryMap->toRegularTexture(GL_NEAREST, GL_CLAMP_TO_EDGE),
                                          textureMap->toRegularTexture(GL_LINEAR, GL_CLAMP_TO_EDGE),
                                          transparencyMap->toRegularTexture(GL_NEAREST, GL_CLAMP_TO_EDGE),
                                          //JobList{jobList, jobList + nbJobs},
                                          regularCanvas);                                      
        });
    }
}

extern "C" INTERFACE_EXPORT void INTERFACE_API OnReleaseEvent()
{
    g_processor_pcc->execute([&] {g_synthesizer_pcc.reset();});
    g_processor_pcc.reset();
    LOG_INFO("OnReleaseEvent");
    g_logStream_pcc.close();
}

extern "C" INTERFACE_EXPORT void INTERFACE_API SetIndirectBufferPtr(void* ptr) {
    g_synthesizer_pcc->setIndirectBufferPtr(ptr);
}

extern "C" INTERFACE_EXPORT void INTERFACE_API SetPositionProperties(void *handle,
                                                                  unsigned width,
                                                                  unsigned height,
                                                                  unsigned fmt)
{
    if (g_processor_pcc && g_synthesizer_pcc)
    {
        g_processor_pcc->execute(
            [&]()
            {
                g_synthesizer_pcc->setPositionTexture(
                    new TextureProperty{handle, width, height, TextureFormat::getGLFromUnityTextureFormat(fmt)});
            });
    }
}


extern "C" INTERFACE_EXPORT void INTERFACE_API SetShadowProperties(void *handle,
                                                                 unsigned width,
                                                                 unsigned height,
                                                                 unsigned fmt,
                                                                 float maxBbox)
{
    if (g_processor_pcc && g_synthesizer_pcc)
    {
        g_processor_pcc->execute(
            [&]()
            {
                g_synthesizer_pcc->setShadowTexture(
                    new TextureProperty{handle, width, height, TextureFormat::getGLFromUnityTextureFormat(fmt)});
            });
    }
}

extern "C" INTERFACE_EXPORT void INTERFACE_API SetMVP(float* MVP) {
    g_synthesizer_pcc->setMVP(MVP);
}


extern "C" INTERFACE_EXPORT void INTERFACE_API SetDecimationLevel(int level) { 
    g_synthesizer_pcc->setDecimationLevel(level); 
}


extern "C" INTERFACE_EXPORT void INTERFACE_API SetNumVertexPerPoint(int num_vert_per_point) {
    g_synthesizer_pcc->setNumVertexPerPoint(num_vert_per_point);
}

extern "C" INTERFACE_EXPORT void INTERFACE_API SetMaxBbox(float size) {
    g_synthesizer_pcc->setMaxBbox(size);
}


extern "C" INTERFACE_EXPORT int INTERFACE_API GetFrameIndex() {
    return g_synthesizer_pcc->m_frameIndex;
}

extern "C" INTERFACE_EXPORT int INTERFACE_API GetGOPIndex() {
    return g_synthesizer_pcc->m_GOPIndex;
}

extern "C" INTERFACE_EXPORT void INTERFACE_API ResetFrameAndGOPIndex() {
    g_synthesizer_pcc->m_frameIndex = 0;
    g_synthesizer_pcc->m_GOPIndex = 0;
}

extern "C" INTERFACE_EXPORT void INTERFACE_API SetTextureProperties(void* handle,
    unsigned width,
    unsigned height,
    unsigned fmt)
{
    if (g_processor_pcc && g_synthesizer_pcc)
    {
        g_processor_pcc->execute(
            [&]()
            {
                g_synthesizer_pcc->setTextureTexture(
                    new TextureProperty{ handle, width, height, TextureFormat::getGLFromUnityTextureFormat(fmt) });
            });
    }
}

extern "C" INTERFACE_EXPORT void INTERFACE_API SetGeometryProperties(void* handle,
    unsigned width,
    unsigned height,
    unsigned fmt)
{
    if (g_processor_pcc && g_synthesizer_pcc)
    {
        g_processor_pcc->execute(
            [&]()
            {
                g_synthesizer_pcc->setGeometryTexture(
                    new TextureProperty{ handle, width, height, TextureFormat::getGLFromUnityTextureFormat(fmt) });
            });
    }
}

extern "C" INTERFACE_EXPORT void INTERFACE_API SetOccupancyProperties(void* handle,
    unsigned width,
    unsigned height,
    unsigned fmt)
{
    if (g_processor_pcc && g_synthesizer_pcc)
    {
        g_processor_pcc->execute(
            [&]()
            {
                g_synthesizer_pcc->setOccupancyTexture(
                    new TextureProperty{ handle, width, height, TextureFormat::getGLFromUnityTextureFormat(fmt) });
            });
    }
}

extern "C" INTERFACE_EXPORT void INTERFACE_API ExportMetadata() {
    if (g_processor_pcc && g_synthesizer_pcc)
    {
        g_processor_pcc->execute(
            [&]()
            {
                g_synthesizer_pcc->ExportMetadata();
            });
    }
}

extern "C" INTERFACE_EXPORT void INTERFACE_API setGlobal3DPatchOffsets(float N_off, float T_off, float B_off)
{
    if (g_processor_pcc && g_synthesizer_pcc)
    {
        g_processor_pcc->execute(
            [&]()
            {
                g_synthesizer_pcc->setGlobal3DPatchOffsets(N_off, T_off, B_off);
            });
    }
}

extern "C" INTERFACE_EXPORT void INTERFACE_API setDynamicDecimation(bool useDD) {
    if (g_processor_pcc && g_synthesizer_pcc)
    {
        g_processor_pcc->execute(
            [&]()
            {
                g_synthesizer_pcc->setDynamicDecimation(useDD);
            });
    }
}

extern "C" INTERFACE_EXPORT void INTERFACE_API setDecimationRanges(float r1, float r2, float vp_cull_factor)
{
    if (g_processor_pcc && g_synthesizer_pcc)
    {
        g_processor_pcc->execute(
            [&]()
            {
                g_synthesizer_pcc->setDecimationRanges(r1,r2, vp_cull_factor);
            });
    }
}

extern "C" INTERFACE_EXPORT void INTERFACE_API setForceRebuild(bool force_rebuild) {
    if (g_processor_pcc && g_synthesizer_pcc)
    {
        g_processor_pcc->execute(
            [&]()
            {
                g_synthesizer_pcc->setForceRebuild(force_rebuild);
            });
    }
}

