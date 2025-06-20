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

#include <cstring>
#include <main/interface.h>
#include <unity/IUnityGraphics.h>

#ifdef _WIN64
#include <unity/IUnityGraphicsD3D11.h>
#elif defined __ANDROID__
#include <EGL/egl.h>
#else
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static std::unique_ptr<Interface> g_interface;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static Video::Backend g_videoBackend = Video::Backend::None;

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetVideoBackend(int backendId)
{
    g_videoBackend = static_cast<Video::Backend>(backendId);
}

auto getVideoBackend() -> Video::Backend { return g_videoBackend; }

auto getFlipMode() -> unsigned
{
    return (g_videoBackend == Video::Backend::D3D11) ? iloj::gpu::FlipMode::Vertical : iloj::gpu::FlipMode::None;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UnitySetInterfaces
static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType);

static IUnityInterfaces *s_UnityInterfaces = nullptr;
static IUnityGraphics *s_Graphics = nullptr;

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces *unityInterfaces)
{
    s_UnityInterfaces = unityInterfaces;
    s_Graphics = s_UnityInterfaces->Get<IUnityGraphics>();
    s_Graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);

    // Run OnGraphicsDeviceEvent(initialize) manually on plugin load
    OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
{
    s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsDeviceEvent

auto getModeFromDeviceType(UnityGfxRenderer deviceType) -> Video::Backend
{
    switch (deviceType)
    {
        case kUnityGfxRendererD3D11:
            return Video::Backend::D3D11;
        case kUnityGfxRendererOpenGLCore:
        case kUnityGfxRendererOpenGLES30:
            return Video::Backend::OpenGL;
        default:
            return Video::Backend::None;
    }
}

inline void DoEventGraphicsDevice(UnityGfxRenderer deviceType, UnityGfxDeviceEventType eventType)
{
    switch (eventType)
    {
        case kUnityGfxDeviceEventInitialize:
        {
            g_videoBackend = getModeFromDeviceType(deviceType);
            break;
        }
        case kUnityGfxDeviceEventShutdown:
        {
            break;
        }
        default:;
    }
}

static UnityGfxRenderer s_DeviceType = kUnityGfxRendererNull;

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
    switch (eventType)
    {
        case kUnityGfxDeviceEventInitialize:
        {
            s_DeviceType = s_Graphics->GetRenderer();
            break;
        }
        case kUnityGfxDeviceEventShutdown:
        {
            /// Here, we want to dispatch before we reset the device type, so the
            /// right device type gets shut down. Thus we return instead of break.
            DoEventGraphicsDevice(s_DeviceType, eventType);
            s_DeviceType = kUnityGfxRendererNull;
            return;
        }
        default:;
    }

    DoEventGraphicsDevice(s_DeviceType, eventType);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rendering stuff

auto getGraphicsHandle() -> void *
{
    switch (getVideoBackend())
    {
#ifdef _WIN64
        case Video::Backend::D3D11:
        {
            return s_UnityInterfaces->Get<IUnityGraphicsD3D11>()->GetDevice();
        }
        case Video::Backend::OpenGL:
        {
            return wglGetCurrentContext();
        }
#elif defined __ANDROID__
        case Video::Backend::OpenGL:
        {
            return eglGetCurrentContext();
        }
#endif
        default:
            return nullptr;
    }
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetGraphicsHandle(int /**/)
{
    if (g_interface)
    {
        if (!g_interface->isReady())
        {
            g_interface->setGraphicsHandle(getGraphicsHandle());
        }
    }
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API OnRenderEvent(int /* eventID */)
{
    if (g_interface)
    {
        if (!g_interface->isReady())
        {
            g_interface->setGraphicsHandle(getGraphicsHandle());
        }

        g_interface->getVideoInterface().onRenderEvent();
        g_interface->getHapticInterface().onRenderEvent();
    }
}

extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc() { return OnRenderEvent; }

extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetGraphicsHandleSetterFunc() { return SetGraphicsHandle; }



extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API CheckPluginStatus()
{
    if (g_interface)
    {
        return g_interface->isReady();
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Create / Destroy

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API OnCreateEvent(char *configFile)
{
    onCreation(configFile);

    g_interface = std::make_unique<Interface>();
    g_interface->onConfigure(configFile);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API OnDestroyEvent()
{
    g_interface.reset();

    onDestroy();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Start / Stop / Quit

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API OnStartEvent(unsigned mediaId)
{
    if (g_interface)
    {
        g_interface->onStartEvent(mediaId);
    }
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API OnPauseEvent(bool b)
{
    if (g_interface)
    {
        g_interface->onPauseEvent(b);
    }
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API OnStopEvent()
{
    if (g_interface)
    {
        g_interface->onStopEvent();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Errors handling
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetOnErrorEventCallback(OnErrorEventCallback ec)
{
    if (g_interface)
    {
        g_interface->setOnErrorEventCallback(ec);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Canvas stuff
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetCanvasProperties(void *handle,
                                                                               unsigned width,
                                                                               unsigned height,
                                                                               unsigned fmt)
{
    if (g_interface)
    {
        g_interface->getVideoInterface().setCanvasProperties(handle, width, height, fmt);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Update audio job

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UpdateAudioExtrinsics(float tx, float ty, float tz, float qx, float qy, float qz, float qw)
{
    if (g_interface)
    {
        g_interface->getAudioInterface().onCameraMotion({tx, ty, tz}, {qx, qy, qz, qw});
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Update video task
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UpdateNumberOfJobs(unsigned nbJobs)
{
    if (g_interface)
    {
        g_interface->getVideoInterface().getJobList().resize(nbJobs);        
    }
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UpdateViewport(unsigned jobId, unsigned w, unsigned h, unsigned left, unsigned bottom)
{
    if (g_interface)
    {
        g_interface->getVideoInterface().getJobList()[jobId].updateViewport(w, h, left, bottom);        
    }
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UpdateCameraProjection(unsigned jobId, unsigned typeId)
{
    if (g_interface)
    {
        g_interface->getVideoInterface().getJobList()[jobId].updateCameraProjection(typeId);
    }
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UpdateCameraResolution(unsigned jobId,
                                                                                  unsigned w,
                                                                                  unsigned h)
{
    if (g_interface)
    {
        g_interface->getVideoInterface().getJobList()[jobId].updateCameraResolution(w, h);
    }
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UpdateCameraIntrinsics(unsigned jobId, float k1, float k2, float k3, float k4)
{
    if (g_interface)
    {
        g_interface->getVideoInterface().getJobList()[jobId].updateCameraIntrinsics(k1, k2, k3, k4);
    }
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UpdateCameraExtrinsics(unsigned jobId, float tx, float ty, float tz, float qx, float qy, float qz, float qw)
{
    if (g_interface)
    {
        g_interface->getVideoInterface().getJobList()[jobId].updateCameraExtrinsics(tx, ty, tz, qx, qy, qz, qw);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Synthesis quality
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetQualityProfile(unsigned profileId)
{
    if (g_interface)
    {
        g_interface->getVideoInterface().setQuality(static_cast<Video::Quality>(profileId));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Media management
extern "C" unsigned UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetNumberOfMedia()
{
    if (g_interface)
    {
        return static_cast<unsigned>(g_interface->getClientInterface().getMediaList().size());
    }

    return 0;
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetMediaName(unsigned mediaId, char *buffer, int bufferSize)
{
    if (g_interface)
    {
        const auto &mediaName = g_interface->getClientInterface().getMediaList()[mediaId];
        size_t count = (mediaName.size() < bufferSize-1) ? mediaName.size() : bufferSize-1;
        mediaName.copy(buffer, count);
        buffer[count] = '\0';
    }
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API OnMediaRequest(unsigned mediaId)
{
    if (g_interface)
    {
        g_interface->onMediaRequest(mediaId);
    }
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetMediaId()
{
    if (g_interface)
    {
        return g_interface->getVideoInterface().getMediaId();
    }

    return -1;
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetMediaType()
{
    if (g_interface)
    {
        return g_interface->getVideoInterface().getMediaType();
    }

    return 0;
}

extern "C" bool UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API IsViewingSpaceCameraIn(float x, float y, float z)
{
    if (g_interface)
    {
        return g_interface->getVideoInterface().isViewingSpaceCameraIn(x, y, z);
    }

    return false;
}

extern "C" float UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetViewingSpaceInclusion(unsigned jobId)
{
    if (g_interface)
    {
        return g_interface->getVideoInterface().getViewingSpaceInclusion(jobId);
    }

    return -1.F;
}

extern "C" float UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetViewingSpaceSize()
{
    if (g_interface)
    {
        return g_interface->getVideoInterface().getViewingSpaceSize();
    }

    return -1.F;
}

extern "C" float UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetViewingSpaceSolidAngle()
{
    if (g_interface)
    {
        return g_interface->getVideoInterface().getViewingSpaceSolidAngle();
    }

    return 0.F;
}

extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetReferenceCameraType()
{
    if (g_interface)
    {
        return g_interface->getVideoInterface().getReferenceCameraType();
    }

    return -1;
}

extern "C" float UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetReferenceCameraAspectRatio()
{
    if (g_interface)
    {
        return g_interface->getVideoInterface().getReferenceCameraAspectRatio();
    }

    return -1;
}

extern "C" float UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetReferenceCameraVerticalFoV()
{
    if (g_interface)
    {
        return g_interface->getVideoInterface().getReferenceCameraVerticalFoV();
    }

    return -1;
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetReferenceCameraClippingRange(float *zMin, float *zMax)
{
    if (g_interface)
    {
        auto range = g_interface->getVideoInterface().getReferenceCameraClippingRange();

        *zMin = range[0];
        *zMax = range[1];
    }
    else
    {
        *zMin = 0.F;
        *zMax = 0.F;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// V3C data polling
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetGenericData(unsigned *frameId,
                                                                      void **metadataPtr,
                                                                      void **occupancyMapId,
                                                                      unsigned *occupancyMapWidth,
                                                                      unsigned *occupancyMapHeight,
                                                                      unsigned *occupancyMapFormat,
                                                                      void **geometryMapId,
                                                                      unsigned *geometryMapWidth,
                                                                      unsigned *geometryMapHeight,
                                                                      unsigned *geometryMapFormat,
                                                                      void **textureMapId,
                                                                      unsigned *textureMapWidth,
                                                                      unsigned *textureMapHeight,
                                                                      unsigned *textureMapFormat,
                                                                      void **transparencyMapId,
                                                                      unsigned *transparencyMapWidth,
                                                                      unsigned *transparencyMapHeight,
                                                                      unsigned *transparencyMapFormat)
{
    if (g_interface)
    {
        auto genericData = g_interface->getVideoInterface().getGenericData();

        *frameId = genericData.frameId;
        *metadataPtr = genericData.metaData;
        *occupancyMapId = genericData.occupancyMap.handle;
        *occupancyMapWidth = genericData.occupancyMap.width;
        *occupancyMapHeight = genericData.occupancyMap.height;
        *occupancyMapFormat = genericData.occupancyMap.format;
        *geometryMapId = genericData.geometryMap.handle;
        *geometryMapWidth = genericData.geometryMap.width;
        *geometryMapHeight = genericData.geometryMap.height;
        *geometryMapFormat = genericData.geometryMap.format;
        *textureMapId = genericData.textureMap.handle;
        *textureMapWidth = genericData.textureMap.width;
        *textureMapHeight = genericData.textureMap.height;
        *textureMapFormat = genericData.textureMap.format;
        *transparencyMapId = genericData.transparencyMap.handle;
        *transparencyMapWidth = genericData.transparencyMap.width;
        *transparencyMapHeight = genericData.transparencyMap.height;
        *transparencyMapFormat = genericData.transparencyMap.format;
    }
    else
    {
        *frameId = 0U;
        *metadataPtr = nullptr;
        *occupancyMapId = 0;
        *occupancyMapWidth = 0;
        *occupancyMapHeight = 0;
        *occupancyMapFormat = 0;
        *geometryMapId = 0;
        *geometryMapWidth = 0;
        *geometryMapHeight = 0;
        *geometryMapFormat = 0;
        *textureMapId = 0;
        *textureMapWidth = 0;
        *textureMapHeight = 0;
        *textureMapFormat = 0;
        *transparencyMapId = 0;
        *transparencyMapWidth = 0;
        *transparencyMapHeight = 0;
        *transparencyMapFormat = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Frame rate data
extern "C" double UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetDecoderFPS()
{
    if (g_interface)
    {
       return g_interface->getDecoderInterface().getDecoderFPS();
    }
    return -1.0;
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API FlushFPSMeasures()
{
    if (g_interface)
    {
        g_interface->getDecoderInterface().flushFPSMeasures();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Point cloud data

//Return last decoded atlas frame height, needed for allocation when rendering outside the plugin
extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetAtlasFrameHeight()
{
    if (g_interface)
    {
        return g_interface->getDecoderInterface().getAtlasFrameHeight();
    }
    return 0;
}

// Return last decoded atlas frame width, needed for allocation when rendering outside the plugin
extern "C" int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetAtlasFrameWidth()
{
    if (g_interface)
    {
        return g_interface->getDecoderInterface().getAtlasFrameWidth();
    }
    return 0;
}
