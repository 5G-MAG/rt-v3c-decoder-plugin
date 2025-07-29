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

#include <iloj/gpu/image.h>
#include <iloj/gpu/renderer.h>
#include <interface/video.h>
#include <common/video/texture.h>

class VideoInterface: public Video::Interface
{
public:
    using OnCreateCallback = void(const char *configFile, unsigned synthesizerId);
    using OnCapabilityCallback = bool(const GenericMetadata *metaData);
    using OnRenderCallback = void(const GenericMetadata *metaData,
                                  const Video::TextureProperty *occupancyMap,
                                  const Video::TextureProperty *geometryMap,
                                  const Video::TextureProperty *textureMap,
                                  const Video::TextureProperty *transparencyMap,
                                  unsigned nbJobs,
                                  const Job *jobList,
                                  const Video::TextureProperty *canvas);
    using OnReleaseCallback = void();
private:

    class Resources
    {
    private:
        std::array<iloj::gpu::image::Importer, VideoStream::Size> m_videoStreamImporter;
        std::array<iloj::gpu::Texture2D, VideoStream::Size> m_videoStreamMap;
        std::array<Video::TextureProperty, VideoStream::Size> m_videoStreamTexture;

    public:
        void import(const DecodedVideoData &data);
        auto getVideoStreamMap(std::size_t streamId) const -> const iloj::gpu::Texture2D &
        {
            return m_videoStreamMap[streamId];
        }
        auto getVideoStreamTexture(std::size_t streamId) const -> const Video::TextureProperty &
        {
            return m_videoStreamTexture[streamId];
        }
    };

    class Synthesizer
    {
    private:
        //Read from config.json (not mandatory)
        Video::Quality m_quality{Video::Quality::None};
        
        //Synthesizers Callbacks (i.e. exposed interface in the synth plugins)

        //These are always expected
        OnCreateCallback *m_onCreateCallback = nullptr;
        OnReleaseCallback *m_onReleaseCallback = nullptr;

        OnCapabilityCallback *m_onCapabilityCallback = nullptr;

        OnRenderCallback *m_onRenderCallback = nullptr;
        
       
    public:
        Synthesizer(const std::string &configFile, unsigned synthesizerId);
        ~Synthesizer();
        Synthesizer(const Synthesizer &) = delete;
        Synthesizer(Synthesizer &&other) noexcept;
        auto operator=(const Synthesizer &other) -> Synthesizer = delete;
        auto operator=(Synthesizer &&other) noexcept -> Synthesizer &;
        auto hasCapability(const GenericMetadata &metadata, Video::Quality quality) const -> bool;
        void renderCanvas(const GenericMetadataPacket &metadataPacket,
                          const Video::TextureProperty &occupancyMap,
                          const Video::TextureProperty &geometryMap,
                          const Video::TextureProperty &textureMap,
                          const Video::TextureProperty &transparencyMap,
                          const std::vector<Job> &jobList,
                          const Video::TextureProperty &canvasTexture) const;
    };

private:
    static HANDLE g_graphicsHandle;
    static std::unique_ptr<iloj::gpu::Processor> g_procRendering;

private:
    std::string m_configFile;
    Video::TextureProperty m_canvasProperty{};

    std::unique_ptr<SharedTexture2D::Base> m_sharedTexture;
    std::unique_ptr<iloj::gpu::Texture2D> m_canvasMap;
    std::unique_ptr<Resources> m_resources;
    GenericMetadataPacket m_metadataPacket;
    DecodedVideoInput m_input;
    std::vector<Synthesizer> m_synthesizerList;
    unsigned m_frameId{};
    void *m_bufferPtr = nullptr;

public:
    ~VideoInterface() override;
    void onGraphicsHandle(HANDLE handle) override;
    auto getSharedOpenGLContext() -> HANDLE override { return g_procRendering->getNativeHandle(); }
    void onConfigure(const std::string &configFile) override;
    void setCanvasProperties(HANDLE handle, unsigned w, unsigned h, unsigned fmt) override;

    void onStartEvent() override;
    void onSampleEvent(const DecodedVideoPacket &pkt) override { m_input.push(pkt); }
    void onRenderEvent() override;
    auto getGenericData() -> Video::GenericData override;
    void onPauseEvent(bool b) override;
    void onStopEvent() override;
    auto getMediaId() -> int override;
    auto getMediaType() -> int override;
    auto isViewingSpaceCameraIn(float x, float y, float z) -> bool override;
    auto getViewingSpaceInclusion(unsigned jobId) -> float override;
    auto getViewingSpaceSize() -> float override;
    auto getViewingSpaceSolidAngle() -> float override;
    auto getReferenceCameraType() -> unsigned override;
    auto getReferenceCameraAspectRatio() -> float override;
    auto getReferenceCameraVerticalFoV() -> float override;
    auto getReferenceCameraClippingRange() -> std::array<float, 2> override;

private:
    void allocateOpenGLContext(HANDLE handle);
    void allocateSharedTexture();
    void allocateResources();
    void fetchMetadata();

};
