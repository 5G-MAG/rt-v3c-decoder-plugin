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

#include <TMIV/ViewingSpace/ViewingSpaceEvaluator.h>
#include <common/video/pose.h>
#include <common/texture_format.h>
#include <iloj/gpu/framework/native/processor.h>
#include <iloj/gui/text.h>
#include <iloj/misc/dll.h>
#include <iloj/misc/filesystem.h>
#include <iloj/misc/json.h>
#include <video/video.h>

using namespace iloj::misc;
using namespace iloj::gpu;
using namespace Video;
using namespace TextureFormat;
////////////////////////////////////////////////////////////////////////////////////////////////////
HANDLE VideoInterface::g_graphicsHandle{};
std::unique_ptr<iloj::gpu::Processor> VideoInterface::g_procRendering;

namespace
{
////////////////////////////////////////////////////////////////////////////////////////////////////
auto getImportMode(int streamId) -> image::Importer::Mode
{
    switch (streamId)
    {
        case VideoStream::Occupancy:
            return image::Importer::Mode::Luma;
        case VideoStream::Geometry:
            return image::Importer::Mode::Luma;
        case VideoStream::Texture:
            return image::Importer::Mode::Color;
        case VideoStream::Transparency:
            return image::Importer::Mode::Luma;
        default:
            return image::Importer::Mode::Luma;
    }
}

auto getColorProfile(int streamId) -> iloj::media::ColorProfile
{
    switch (streamId)
    {
        case VideoStream::Occupancy:
            return iloj::media::ColorProfile::BT709_FullRange();
        case VideoStream::Geometry:
            return iloj::media::ColorProfile::BT709_FullRange();
        case VideoStream::Texture:
            return iloj::media::ColorProfile::BT709();
        case VideoStream::Transparency:
            return iloj::media::ColorProfile::BT709_FullRange();
        default:
            return iloj::media::ColorProfile::None();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
auto getTexturePropertyfromRegularTexture(const iloj::gpu::Texture2D &tex) -> Video::TextureProperty
{
    return {reinterpret_cast<HANDLE>(static_cast<std::size_t>(tex.getId())),
            tex.width(),
            tex.height(),
            tex.getInternalFormat()};
}

} // namespace

void VideoInterface::Resources::import(const DecodedVideoData &data)
{
    for (auto streamId = 0; streamId < VideoStream::Size; streamId++)
    {
        if (data.videoPacketList[streamId])
        {
            m_videoStreamImporter[streamId].load((data.videoPacketList[streamId]).getContent(),
                                                 m_videoStreamMap[streamId],
                                                 getImportMode(streamId),
                                                 FlipMode::Vertical,
                                                 GL_NEAREST,
                                                 GL_CLAMP_TO_EDGE,
                                                 getColorProfile(streamId));

            m_videoStreamTexture[streamId] = getTexturePropertyfromRegularTexture(m_videoStreamMap[streamId]);
        }
        else
        {
            m_videoStreamTexture[streamId] = {};
        }
    }

    glFinish();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
VideoInterface::Synthesizer::Synthesizer(const std::string &configFile, unsigned synthesizerId)
{
    auto json =
        JSON::Object::fromFile(configFile).getItem<JSON::Array>("RendererList").getItem<JSON::Object>(synthesizerId);
    auto modulePath = json.getItem<JSON::String>("Module").getValue();

    if (json.hasItem("Quality"))
    {
        auto quality = json.getItem<JSON::String>("Quality").getValue();

        if (quality == "low")
        {
            m_quality = Video::Quality::Low;
        }
        else if (quality == "medium")
        {
            m_quality = Video::Quality::Medium;
        }
        else if (quality == "high")
        {
            m_quality = Video::Quality::High;
        }
    }

    LoadProcEx(modulePath, "OnCreateEvent", m_onCreateCallback);
    LoadProcEx(modulePath, "OnCapabilityEvent", m_onCapabilityCallback);
    LoadProcEx(modulePath, "OnRenderEvent", m_onRenderCallback);
    LoadProcEx(modulePath, "OnReleaseEvent", m_onReleaseCallback);    

    if (m_onCreateCallback)
    {
        m_onCreateCallback(configFile.c_str(), synthesizerId);
        LOG_INFO("Synthesizer created [", synthesizerId, "] (quality profile: ", static_cast<unsigned>(m_quality), ")");
    }
}

VideoInterface::Synthesizer::~Synthesizer()
{
    if (m_onReleaseCallback)
    {
        m_onReleaseCallback();
        LOG_INFO("Synthesizer released");
    }
}

VideoInterface::Synthesizer::Synthesizer(VideoInterface::Synthesizer &&other) noexcept
{
    m_onCreateCallback = other.m_onCreateCallback;
    m_onCapabilityCallback = other.m_onCapabilityCallback;
    m_onRenderCallback = other.m_onRenderCallback;
    m_onReleaseCallback = other.m_onReleaseCallback;
 
    other.m_onCreateCallback = nullptr;
    other.m_onCapabilityCallback = nullptr;
    other.m_onRenderCallback = nullptr;
    other.m_onReleaseCallback = nullptr;
   
}

auto VideoInterface::Synthesizer::operator=(VideoInterface::Synthesizer &&other) noexcept -> Synthesizer &
{
    m_onCreateCallback = other.m_onCreateCallback;
    m_onCapabilityCallback = other.m_onCapabilityCallback;
    m_onRenderCallback = other.m_onRenderCallback;
    m_onReleaseCallback = other.m_onReleaseCallback;
   
    other.m_onCreateCallback = nullptr;
    other.m_onCapabilityCallback = nullptr;
    other.m_onRenderCallback = nullptr;
    other.m_onReleaseCallback = nullptr;

    return *this;
}

auto VideoInterface::Synthesizer::hasCapability(const GenericMetadata &metadata, Video::Quality quality) const -> bool
{
    
    if ((m_quality == Video::Quality::None) || (m_quality == quality))
    {
       if (m_onCapabilityCallback != nullptr)
       {
           return m_onCapabilityCallback(&metadata); 
       }
            
    }
    else
    {
        return false;
    }

    return false;
}

void VideoInterface::Synthesizer::renderCanvas(const GenericMetadataPacket &metadataPacket,
                                               const Video::TextureProperty &occupancyMap,
                                               const Video::TextureProperty &geometryMap,
                                               const Video::TextureProperty &textureMap,
                                               const Video::TextureProperty &transparencyMap,
                                               const std::vector<Job> &jobList,
                                               const Video::TextureProperty &canvasTexture) const
{
    if (m_onRenderCallback)
    {
        m_onRenderCallback(metadataPacket ? &metadataPacket.getContent() : nullptr,
                           &occupancyMap,
                           &geometryMap,
                           &textureMap,
                           &transparencyMap,
                           static_cast<unsigned>(jobList.size()),
                           jobList.data(),
                           &canvasTexture);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
VideoInterface::~VideoInterface()
{
    if (g_procRendering)
    {
        g_procRendering->execute(
            [this]()
            {
                m_synthesizerList.clear();
                m_canvasMap.reset();
                m_sharedTexture.reset();
                m_resources.reset();
            });
    }

    LOG_INFO("VideoInterface::~VideoInterface");
}

void VideoInterface::onGraphicsHandle(HANDLE handle)
{
    LOG_INFO("VideoInterface::onGraphicsHandle");

    allocateOpenGLContext(handle);

    if (g_procRendering)
    {
        g_procRendering->execute(
            [this]()
            {
                allocateResources();

                auto nbSynthesizer =
                    JSON::Object::fromFile(m_configFile).getItem<JSON::Array>("RendererList").getSize();

                for (auto synthesizerId = 0U; synthesizerId < nbSynthesizer; synthesizerId++)
                {
                    m_synthesizerList.emplace_back(m_configFile, static_cast<unsigned>(synthesizerId));
                }
            });
    }
}

void VideoInterface::onConfigure(const std::string &configFile)
{
    LOG_INFO("VideoInterface::onConfigure ", configFile);
    
    auto config = JSON::Object::fromFile(configFile).getItem<JSON::Object>("Scheduler");
    if (auto &item = config.getItem("FrameSkip"))
    {
        m_frameSkip = item.as<bool>();
    }
    

    m_configFile = configFile;
}

void VideoInterface::setCanvasProperties(HANDLE handle, unsigned w, unsigned h, unsigned fmt)
{
    m_canvasProperty = {handle, w, h, fmt};
}


void VideoInterface::onStartEvent()
{
    LOG_INFO("VideoInterface::onStartEvent");

    m_input.open();
}

void VideoInterface::onRenderEvent()
{
    // LOG_INFO("VideoInterface::onRenderEvent");

    if (g_procRendering && m_canvasProperty.handle)
    {
        g_procRendering->execute(
            [this]
            {
                if (!m_sharedTexture || (m_canvasProperty.handle != m_sharedTexture->getHandle()))
                {
                    allocateSharedTexture();
                }

                if (!m_input.empty())
                {
                   
                    auto &data = m_frameSkip ? m_input.back() : m_input.front(); 
                    m_metadataPacket = data->metadataPacket;
                    m_resources->import(data.getContent());

                    m_input.pop();
                }

                auto iter =
                    m_metadataPacket
                        ? std::find_if(m_synthesizerList.begin(),
                                       m_synthesizerList.end(),
                                       [&](const Synthesizer &synthesizer)
                                       { return synthesizer.hasCapability(m_metadataPacket.getContent(), m_quality); })
                        : m_synthesizerList.end();

                m_sharedTexture->lock();

                auto canvasTexture = getTexturePropertyfromRegularTexture(*m_canvasMap);

                if (iter != m_synthesizerList.end())
                {
                    iter->renderCanvas(m_metadataPacket,
                                       m_resources->getVideoStreamTexture(VideoStream::Occupancy),
                                       m_resources->getVideoStreamTexture(VideoStream::Geometry),
                                       m_resources->getVideoStreamTexture(VideoStream::Texture),
                                       m_resources->getVideoStreamTexture(VideoStream::Transparency),
                                       m_jobList,
                                       canvasTexture);
                }
                else
                {
                    using namespace iloj::gui;

                    clear({*m_canvasMap}, m_canvasMap->getViewPort(), Clear::Context::Color());
                    print(*m_canvasMap,
                          m_canvasMap->getViewPort(),
                          m_metadataPacket ? "No valid synthesizer" : "No valid content",
                          {0.F, 1.F},
                          24.F,
                          {-1.F, -1.F},
                          Alignment::Center);
                }

                m_sharedTexture->unlock();

                if (m_metadataPacket)
                {
                    ++(m_metadataPacket->MIVMetadata->foc);
                }
            });
    }
}

auto VideoInterface::getGenericData() -> Video::GenericData
{
    if (g_procRendering)
    {
        g_procRendering->execute(
            [this]
            {
                if (!m_input.empty())
                {
                    const auto &metadataPacket = m_input.front()->metadataPacket;

                    m_frameId++;

                    if (m_metadataPacket)
                    {
                        if ((m_metadataPacket->contentId != metadataPacket->contentId) ||
                            (m_metadataPacket->segmentId != metadataPacket->segmentId))
                        {
                            m_frameId = 0U;
                        }
                        else
                        {
                            ++(m_metadataPacket->MIVMetadata->foc);
                        }
                    }

                    m_metadataPacket = metadataPacket;
                    m_resources->import(m_input.front().getContent());

                    m_input.pop();
                }
            });
    }

    if (m_metadataPacket)
    {
        auto occupancyMap = m_resources->getVideoStreamTexture(VideoStream::Occupancy);
        auto geometryMap = m_resources->getVideoStreamTexture(VideoStream::Geometry);
        auto textureMap = m_resources->getVideoStreamTexture(VideoStream::Texture);
        auto transparencyMap = m_resources->getVideoStreamTexture(VideoStream::Transparency);

        occupancyMap.format = getUnityFromGLTextureFormat(occupancyMap.format);
        geometryMap.format = getUnityFromGLTextureFormat(geometryMap.format);
        textureMap.format = getUnityFromGLTextureFormat(textureMap.format);
        transparencyMap.format = getUnityFromGLTextureFormat(transparencyMap.format);

        return {std::addressof(m_metadataPacket.getContent()),
                m_frameId,
                occupancyMap,
                geometryMap,
                textureMap,
                transparencyMap};
    }

    return {};
}

void VideoInterface::onPauseEvent(bool b)
{
    if (b)
    {
        m_input.close();
        m_input.clear();

        LOG_INFO("VideoInterface::onPauseEvent");
    }
    else
    {
        m_input.open();

        LOG_INFO("VideoInterface::onResumeEvent");
    }
}

void VideoInterface::onStopEvent()
{
    m_input.close();
    m_input.clear();
    m_metadataPacket.reset();
    LOG_INFO("VideoInterface::onStopEvent");
}

auto VideoInterface::getMediaId() -> int
{
    fetchMetadata();

    if (m_metadataPacket)
    {
        return m_metadataPacket->contentId;
    }
    return -1;
}

auto VideoInterface::getMediaType() -> int
{
    fetchMetadata();

    if (m_metadataPacket)
    {
        return (int)m_metadataPacket->contentType;
    }

    return -1;
}

auto VideoInterface::isViewingSpaceCameraIn(float x, float y, float z) -> bool
{
    fetchMetadata();

    if (m_metadataPacket && m_metadataPacket->MIVMetadata->vs && m_metadataPacket->MIVMetadata->vp &&
        m_metadataPacket->contentType == GenericMetadata::ContentType::MIV)
    {
        const auto& vs = *m_metadataPacket->MIVMetadata->vs;
        const auto& vp = *m_metadataPacket->MIVMetadata->vp;

        const auto& q_ref = vp.vp_orientation();
        const auto& t_ref = vp.vp_position;

        const TMIV::MivBitstream::Pose& poseUnity{ {x, y, z}, TMIV::Common::neutralOrientationD };

        iloj::math::Pose<float> refMiv = { {q_ref.x(), q_ref.y(), q_ref.z(), q_ref.w()},
                                          {t_ref.x(), t_ref.y(), t_ref.z()} };
        auto relMiv = getMivPoseFromUnityPose({ poseUnity.orientation, poseUnity.position });
        auto absMiv = getAbsolutePoseFromMiv(refMiv, relMiv);

        const auto& q = absMiv.getQuaternion();
        const auto& t = absMiv.getTranslation();

        return (
            TMIV::ViewingSpace::ViewingSpaceEvaluator::computeInclusion(vs, 
                { {t.x(), t.y(), t.z()}, {q.x(), q.y(), q.z(), q.w()} }) > 0.0f);
    }

    return true;
}

auto VideoInterface::getViewingSpaceInclusion(unsigned jobId) -> float
{
    fetchMetadata();

    if (m_metadataPacket && m_metadataPacket->MIVMetadata->vs && m_metadataPacket->MIVMetadata->vp &&
        (jobId < m_jobList.size()) &&
        m_metadataPacket->contentType == GenericMetadata::ContentType::MIV)
    {
        const auto& vs = *m_metadataPacket->MIVMetadata->vs;
        const auto& vp = *m_metadataPacket->MIVMetadata->vp;

        const auto& q_ref = vp.vp_orientation();
        const auto& t_ref = vp.vp_position;

        const auto& poseUnity = m_jobList[jobId].camera.pose;

        iloj::math::Pose<float> refMiv = { {q_ref.x(), q_ref.y(), q_ref.z(), q_ref.w()},
                                          {t_ref.x(), t_ref.y(), t_ref.z()} };
        auto relMiv = getMivPoseFromUnityPose({ poseUnity.orientation, poseUnity.position });
        auto absMiv = getAbsolutePoseFromMiv(refMiv, relMiv);

        const auto& q = absMiv.getQuaternion();
        const auto& t = absMiv.getTranslation();

        return TMIV::ViewingSpace::ViewingSpaceEvaluator::computeInclusion(
            vs, { {t.x(), t.y(), t.z()}, {q.x(), q.y(), q.z(), q.w()} });
    }

    return 1.F;
}

auto VideoInterface::getViewingSpaceSize() -> float
{
    fetchMetadata();

    if (m_metadataPacket && m_metadataPacket->MIVMetadata->vs &&
        m_metadataPacket->contentType == GenericMetadata::ContentType::MIV)
    {
        // Handle only cuboid & spheroid, x parameter for now
        const auto& vs = *m_metadataPacket->MIVMetadata->vs;
        for (size_t e = 0; e <= vs.vs_num_elementary_shapes_minus1(); e++) {
            auto es = vs.elementary_shape(e);
            for (uint8_t s = 0; s <= es.es_num_primitive_shapes_minus1(); s++)
            {
                auto ps = es.primitives[s];
                if (es.es_primitive_shape_type(s) == TMIV::MivBitstream::PrimitiveShapeType::cuboid &&
                    std::holds_alternative<TMIV::MivBitstream::Cuboid>(ps.primitive))
                {
                    // Complete with y & z when needed
                    auto cuboid = std::get<TMIV::MivBitstream::Cuboid>(ps.primitive);
                    return cuboid.cp_size_x();
                }
                else if (es.es_primitive_shape_type(s) == TMIV::MivBitstream::PrimitiveShapeType::spheroid &&
                    std::holds_alternative<TMIV::MivBitstream::Spheroid>(ps.primitive))
                {
                    // Complete with y & z when needed
                    auto spheroid = std::get<TMIV::MivBitstream::Spheroid>(ps.primitive);
                    return spheroid.sp_radius_x();
                }
            }
        }
    }

    return -1.f;
}

auto VideoInterface::getViewingSpaceSolidAngle() -> float
{
    fetchMetadata();
    
    if (m_metadataPacket && m_metadataPacket->MIVMetadata->vs)
    {
        const auto &vs = *m_metadataPacket->MIVMetadata->vs;

        if (vs.vs_num_elementary_shapes_minus1() == 0)
        {
            const auto &eso = vs.elementaryShapes.front();

            if ((eso.elementary_shape.es_num_primitive_shapes_minus1() == 0) &&
                (eso.elementary_shape_operation == TMIV::MivBitstream::ElementaryShapeOperation::add))
            {
                const auto &ps = eso.elementary_shape.primitives.front();

                if (ps.viewingDirectionConstraint)
                {
                    const auto &vdc = *ps.viewingDirectionConstraint;

                    float theta = 0.5F * iloj::math::deg2rad(vdc.yawRange);
                    float phi = 0.5F * iloj::math::deg2rad(vdc.pitchRange);

                    return iloj::math::solid(-theta, theta, -phi, phi);
                }
            }
        }
    }

    return 0.F;
}

auto VideoInterface::getReferenceCameraType() -> unsigned
{    
    fetchMetadata();

    if (m_metadataPacket && m_metadataPacket->MIVMetadata->vcp)
    {
        return static_cast<unsigned>(m_metadataPacket->MIVMetadata->vcp->vcp_camera_type);
    }

    return static_cast<unsigned>(TMIV::MivBitstream::CiCamType::perspective);
}

auto VideoInterface::getReferenceCameraAspectRatio() -> float
{
    fetchMetadata();
    

    if (m_metadataPacket && m_metadataPacket->MIVMetadata->vcp)
    {
        const auto &vcp = *m_metadataPacket->MIVMetadata->vcp;

        switch (vcp.vcp_camera_type)
        {
            case TMIV::MivBitstream::CiCamType::equirectangular:
                return vcp.vcp_erp_horizontal_fov_in_degrees() / vcp.vcp_erp_vertical_fov_in_degrees();
            case TMIV::MivBitstream::CiCamType::perspective:
                return vcp.vcp_perspective_aspect_ratio;
            case TMIV::MivBitstream::CiCamType::orthographic:
                return vcp.vcp_ortho_aspect_ratio;
        }
    
    }
    else if (m_resources)
    {
        const auto &textureProperty = m_resources->getVideoStreamTexture(VideoStream::Texture);

        if (0 < textureProperty.height)
        {
            return static_cast<float>(textureProperty.width) / static_cast<float>(textureProperty.height);
        }
    }

    return 0.;
}

auto VideoInterface::getReferenceCameraVerticalFoV() -> float
{
    fetchMetadata();

    if (m_metadataPacket && m_metadataPacket->MIVMetadata->vcp)
    {
        LOG_INFO("FOV from vcp");
        const auto &vcp = *m_metadataPacket->MIVMetadata->vcp;

        switch (vcp.vcp_camera_type)
        {
            case TMIV::MivBitstream::CiCamType::equirectangular:
                return iloj::math::deg2rad(vcp.vcp_erp_vertical_fov_in_degrees());
            case TMIV::MivBitstream::CiCamType::perspective:
                return iloj::math::deg2rad(vcp.vcp_perspective_vertical_fov_in_degrees());
            case TMIV::MivBitstream::CiCamType::orthographic:
                return K_PI;
        }
    }
    else if (m_resources)
    {
        //LOG_INFO("Trying FOV from ressources");
        const auto &textureProperty = m_resources->getVideoStreamTexture(VideoStream::Texture);

        if (0 < textureProperty.height)
        {
            //LOG_INFO("FOV from ressources");
            return iloj::math::deg2rad(60.F);
        }
    }
    //LOG_INFO("FOV to Default");
    return iloj::math::deg2rad(60.F);
}

auto VideoInterface::getReferenceCameraClippingRange() -> std::array<float, 2>
{
    fetchMetadata();

    if (m_metadataPacket && m_metadataPacket->MIVMetadata->vcp)
    {
        const auto &vcp = *m_metadataPacket->MIVMetadata->vcp;
        return {vcp.vcp_clipping_near_plane, vcp.vcp_clipping_far_plane};
    }
    else if (m_resources)
    {
        const auto &textureProperty = m_resources->getVideoStreamTexture(VideoStream::Texture);

        if (0 < textureProperty.height)
        {
            return {1.F, 1000.F};
        }
    }

    return {};
}

void VideoInterface::allocateOpenGLContext(HANDLE handle)
{
    if (!g_graphicsHandle)
    {
        g_graphicsHandle = handle;

        switch (getVideoBackend())
        {
            case Backend::D3D11:
            {
                g_procRendering = std::make_unique<iloj::gpu::framework::native::Processor>();
                LOG_INFO("GL rendering processor allocated (D3D11)");
                break;
            }
            case Backend::OpenGL:
            {
#ifdef _WIN64
                g_procRendering = std::make_unique<iloj::gpu::framework::native::Processor>(
                    reinterpret_cast<HGLRC>(g_graphicsHandle), true);
                //g_procRendering->disableContext();
#elif defined __ANDROID__
                g_procRendering = std::make_unique<iloj::gpu::framework::native::Processor>(
                    reinterpret_cast<EGLContext>(g_graphicsHandle), true);
#endif
                LOG_INFO("GL rendering processor allocated (OpenGL)");

                break;
            }
            default:
            {
                LOG_ERROR("Invalid mode: cannot allocate context");
            }
        }
    }
}

void VideoInterface::allocateResources()
{
    m_resources = std::make_unique<Resources>();

    LOG_INFO("Resources allocated");
}

void VideoInterface::allocateSharedTexture()
{
    switch (getVideoBackend())
    {
#ifdef _WIN64
        case Backend::D3D11:
        {
            m_sharedTexture = std::make_unique<SharedTexture2D::D3D11>(g_graphicsHandle, m_canvasProperty.handle);
            break;
        }
#endif
        case Backend::OpenGL:
        {
            m_sharedTexture = std::make_unique<SharedTexture2D::OpenGL>(
                static_cast<GLuint>(reinterpret_cast<std::size_t>(m_canvasProperty.handle)));
            break;
        }
        default:;
    }

    m_canvasMap = std::make_unique<Texture2D>(m_sharedTexture->getGLName(),
                                              m_canvasProperty.width,
                                              m_canvasProperty.height,
                                              getGLFromUnityTextureFormat(m_canvasProperty.format),
                                              GL_NONE,
                                              GL_NONE);

    LOG_INFO("Shared texture allocated");
}

void VideoInterface::fetchMetadata() {
    if (!m_metadataPacket)
    {
        if (!m_input.empty())
        {
            m_metadataPacket = m_input.front()->metadataPacket;
        }
    }
}
