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

#include <common/misc/types.h>
#include <common/video/job.h>

namespace Video
{
enum class Backend
{
    None,
    D3D11,
    OpenGL
};

struct TextureProperty
{
    HANDLE handle{};
    unsigned width{};
    unsigned height{};
    unsigned format{};
};

struct GenericData
{
    HANDLE metaData{};
    unsigned frameId{};
    TextureProperty occupancyMap;
    TextureProperty geometryMap;
    TextureProperty textureMap;
    TextureProperty transparencyMap;
};

enum class Quality : unsigned
{
    None,
    Low,
    Medium,
    High
};

class Interface
{
protected:
    Quality m_quality{Quality::None};
    JobList m_jobList{};
    bool m_frameSkip = true;

public:
    Interface() = default;
    virtual ~Interface() = default;
    Interface(const Interface &) = delete;
    Interface(Interface &&other) noexcept = default;
    auto operator=(const Interface &) -> Interface & = delete;
    auto operator=(Interface &&) noexcept -> Interface & = default;
    void setQuality(Quality quality) { m_quality = quality; }
    auto getJobList() -> JobList & { return m_jobList; }
    virtual void onGraphicsHandle(HANDLE handle) = 0;
    virtual auto getSharedOpenGLContext() -> HANDLE = 0;
    virtual void onConfigure(const std::string &configFile) = 0;
    virtual void setCanvasProperties(HANDLE handle, unsigned w, unsigned h, unsigned fmt) = 0;    
    virtual void onStartEvent() = 0;
    virtual void onSampleEvent(const DecodedVideoPacket &pkt) = 0;
    virtual void onRenderEvent() = 0;
    virtual auto getGenericData() -> GenericData = 0;
    virtual void onPauseEvent(bool b) = 0;
    virtual void onStopEvent() = 0;
    virtual auto getMediaId() -> int = 0;
    virtual auto getMediaType() -> int = 0;
    virtual auto isViewingSpaceCameraIn(float x, float y, float z) -> bool = 0;
    virtual auto getViewingSpaceInclusion(unsigned jobId) -> float = 0;
    virtual auto getViewingSpaceSize() -> float = 0;
    virtual auto getViewingSpaceSolidAngle() -> float = 0;
    virtual auto getReferenceCameraType() -> unsigned = 0;
    virtual auto getReferenceCameraAspectRatio() -> float = 0;
    virtual auto getReferenceCameraVerticalFoV() -> float = 0;
    virtual auto getReferenceCameraClippingRange() -> std::array<float, 2> = 0;    
};

} // namespace Video

auto getVideoBackend() -> Video::Backend;
