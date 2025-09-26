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

#include <audio/buffer.h>
#include <cstring>
#include <fstream>
#include <iloj/misc/filesystem.h>
#include <iloj/misc/logger.h>
#include <mutex>
#include <sstream>
#include <unity/AudioPluginInterface.h>
#include <vector>

static std::ofstream g_logStream;
static AudioBuffer g_audioBuffer;

extern "C" UNITY_AUDIODSP_EXPORT_API void AUDIO_CALLING_CONVENTION OnCreateEvent(char *configFile)
{
    using namespace iloj::misc;

    g_logStream.open(FileSystem::Path{configFile}.getParent().toString() +
                     "/AudioPlugin_V3CImmersiveDecoderAudio.log");

    Logger::getInstance().setStream(g_logStream);
    Logger::getInstance().setLevel(Logger::Level::Info);
    Logger::getInstance().enableFlushOnAppend(true);

    LOG_INFO("OnCreateEvent");
}

extern "C" UNITY_AUDIODSP_EXPORT_API void AUDIO_CALLING_CONVENTION OnDestroyEvent()
{
    LOG_INFO("OnDestroyEvent");
    g_audioBuffer.clear();
    g_logStream.close();
}

extern "C" UNITY_AUDIODSP_EXPORT_API void AUDIO_CALLING_CONVENTION OnStartEvent(unsigned mediaId)
{
    LOG_INFO("OnStartEvent Media ", mediaId);
}

extern "C" UNITY_AUDIODSP_EXPORT_API void AUDIO_CALLING_CONVENTION OnCameraMotion(float /* tx */,
                                                                                  float /* ty */,
                                                                                  float /* tz */,
                                                                                  float /* qx */,
                                                                                  float /* qy */,
                                                                                  float /* qz */,
                                                                                  float /* qw */)
{
    // std::ostringstream stream;

    // stream << "OnCameraMotion" << '\n';
    // stream << "\ttx: " << tx << '\n';
    // stream << "\tty: " << ty << '\n';
    // stream << "\ttz: " << tz << '\n';
    // stream << "\tqx: " << qx << '\n';
    // stream << "\tqy: " << qy << '\n';
    // stream << "\tqz: " << qz << '\n';
    // stream << "\tqw: " << qw;

    // LOG(stream.str());
}

extern "C" UNITY_AUDIODSP_EXPORT_API void AUDIO_CALLING_CONVENTION OnSampleEvent(unsigned formatId,
                                                                                 unsigned packingId,
                                                                                 unsigned nbChannels,
                                                                                 unsigned sampleRate,
                                                                                 const void *buffer,
                                                                                 unsigned length)
{
    // std::ostringstream stream;

    // stream << "OnSampleEvent " << '\n';
    // stream << "\tformatId: " << formatId << '\n';
    // stream << "\tpackingId: " << packingId << '\n';
    // stream << "\tnbChannels: " << nbChannels << '\n';
    // stream << "\tsampleRate: " << sampleRate << '\n';
    // stream << "\tbuffer: " << buffer << '\n';
    // stream << "\tlength: " << length;

    // LOG(stream.str());

    g_audioBuffer.push(formatId, packingId, nbChannels, sampleRate, buffer, length);
}

extern "C" UNITY_AUDIODSP_EXPORT_API void AUDIO_CALLING_CONVENTION OnPauseEvent(bool b)
{
    if (b)
    {
        g_audioBuffer.mute(true);
        LOG_INFO("OnPauseEvent");
    }
    else
    {
        g_audioBuffer.mute(false);
        LOG_INFO("OnResumeEvent");
    }
}

extern "C" UNITY_AUDIODSP_EXPORT_API void AUDIO_CALLING_CONVENTION OnStopEvent() 
{ 
    g_audioBuffer.clear();
    LOG_INFO("OnStopEvent");
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK
UnityAudioEffect_CreateCallback_Plugin(UnityAudioEffectState * /* state */)
{
    LOG_INFO("UnityAudioEffect_CreateCallback");

    return 0;
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK
UnityAudioEffect_ReleaseCallback_Plugin(UnityAudioEffectState * /* state */)
{
    LOG_INFO("UnityAudioEffect_ReleaseCallback");

    return 0;
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK UnityAudioEffect_ResetCallback_Plugin(UnityAudioEffectState * /* state */)
{
    LOG_INFO("UnityAudioEffect_ResetCallback");

    return 0;
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK
UnityAudioEffect_ProcessCallback_Plugin(UnityAudioEffectState * /* state */,
                                        float * /* inbuffer */,
                                        float *outbuffer,
                                        unsigned int length,
                                        int /* inchannels */,
                                        int /* outchannels */)
{
    // std::ostringstream stream;

    // stream << "UnityAudioEffect_ProcessCallback " << '\n';
    // stream << "\tnbChannels: " << outchannels << '\n';
    // stream << "\tbuffer: " << outbuffer << '\n';
    // stream << "\tlength: " << length;

    // LOG(stream.str());
    g_audioBuffer.pop(outbuffer, length);

    return 0;
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK
UnityAudioEffect_SetPositionCallback_Plugin(UnityAudioEffectState * /* state */, unsigned int /* pos */)
{
    // TODO
    return 0;
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK
UnityAudioEffect_SetFloatParameterCallback_Plugin(UnityAudioEffectState * /* state */,
                                                  int /* index */,
                                                  float /* value */)
{
    // TODO
    return 0;
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK
UnityAudioEffect_GetFloatParameterCallback_Plugin(UnityAudioEffectState * /* state */,
                                                  int /* index */,
                                                  float * /* value */,
                                                  char * /* valuestr */)
{
    // TODO
    return 0;
}

UNITY_AUDIODSP_RESULT UNITY_AUDIODSP_CALLBACK
UnityAudioEffect_GetFloatBufferCallback_Plugin(UnityAudioEffectState * /* state */,
                                               const char * /* name */,
                                               float * /* buffer */,
                                               int /* numsamples */)
{
    // TODO
    return 0;
}

extern "C" UNITY_AUDIODSP_EXPORT_API int AUDIO_CALLING_CONVENTION
UnityGetAudioEffectDefinitions(UnityAudioEffectDefinition ***descptr)
{
    static UnityAudioEffectDefinition effect;
    static UnityAudioEffectDefinition *pEffect = &effect;

    static char effectName[] = "V3C Decoder Audio Output";

    memset(pEffect, 0, sizeof(effect));
#ifdef _WIN64
    strcpy_s(effect.name, effectName);
#elif defined __ANDROID__
    strcpy(effect.name, effectName);
#else
    // TODO
#endif

    effect.structsize = sizeof(UnityAudioEffectDefinition);
    effect.paramstructsize = sizeof(UnityAudioParameterDefinition);
    effect.apiversion = UNITY_AUDIO_PLUGIN_API_VERSION;
    effect.pluginversion = 0x010000;
    effect.channels = 2;
    effect.numparameters = 0;
    effect.create = UnityAudioEffect_CreateCallback_Plugin;
    effect.release = UnityAudioEffect_ReleaseCallback_Plugin;
    effect.process = UnityAudioEffect_ProcessCallback_Plugin;
    effect.setfloatparameter = UnityAudioEffect_SetFloatParameterCallback_Plugin;
    effect.getfloatparameter = UnityAudioEffect_GetFloatParameterCallback_Plugin;
    effect.getfloatbuffer = UnityAudioEffect_GetFloatBufferCallback_Plugin;

    *descptr = &pEffect;

    return 1;
}
