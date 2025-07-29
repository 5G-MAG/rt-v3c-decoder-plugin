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

#include "interface/audio.h"
#include "interface/client.h"
#include "interface/decoder.h"
#include "interface/scheduler.h"
#include "interface/video.h"
#include "interface/haptic.h"
#include <iloj/misc/logger.h>

void onCreation(const std::string &configFile);
void onDestroy();

auto allocateClientInterface() -> std::unique_ptr<Client::Interface>;
auto allocateDecoderInterface() -> std::unique_ptr<Decoder::Interface>;
auto allocateSchedulerInterface() -> std::unique_ptr<Scheduler::Interface>;
auto allocateAudioInterface() -> std::unique_ptr<Audio::Interface>;
auto allocateVideoInterface() -> std::unique_ptr<Video::Interface>;
auto allocateHapticInterface() -> std::unique_ptr<Haptic::Interface>;

class Interface
{
private:
    bool m_ready{false};
    std::unique_ptr<Client::Interface> m_clientInterface;
    std::unique_ptr<Decoder::Interface> m_decoderInterface;
    std::unique_ptr<Scheduler::Interface> m_schedulerInterface;
    std::unique_ptr<Audio::Interface> m_audioInterface;
    std::unique_ptr<Video::Interface> m_videoInterface;
    std::unique_ptr<Haptic::Interface> m_hapticInterface;

public:
    Interface()
        : m_clientInterface{allocateClientInterface()},
          m_decoderInterface{allocateDecoderInterface()},
          m_schedulerInterface{allocateSchedulerInterface()},
          m_audioInterface{allocateAudioInterface()},
          m_videoInterface{allocateVideoInterface()},
          m_hapticInterface{allocateHapticInterface()}
    {
        m_clientInterface->setDecoderInterface(m_decoderInterface.get());
        m_decoderInterface->setSchedulerInterface(m_schedulerInterface.get());
        m_schedulerInterface->setAudioInterface(m_audioInterface.get());
        m_schedulerInterface->setVideoInterface(m_videoInterface.get());
        m_schedulerInterface->setHapticInterface(m_hapticInterface.get());
    }
    Interface(const Interface &other) = delete;
    Interface(Interface &&other) noexcept = default;
    auto operator=(const Interface &) -> Interface & = delete;
    auto operator=(Interface &&other) noexcept -> Interface & = default;
    auto isReady() const -> bool { return m_ready; }
    void setGraphicsHandle(HANDLE graphicsHandle)
    {
        if (graphicsHandle != nullptr)
        {
            m_videoInterface->onGraphicsHandle(graphicsHandle);
            m_decoderInterface->setSharedOpenGLContext(m_videoInterface->getSharedOpenGLContext());

            m_ready = true;
        }
        else
        {
            LOG_ERROR("Unsupported platform and / or video mode");
        }
    }
    auto getClientInterface() -> Client::Interface & { return *m_clientInterface; }
    auto getClientInterface() const -> const Client::Interface & { return *m_clientInterface; }
    auto getDecoderInterface() -> Decoder::Interface & { return *m_decoderInterface; }
    auto getDecoderInterface() const -> const Decoder::Interface & { return *m_decoderInterface; }
    auto getSchedulerInterface() -> Scheduler::Interface & { return *m_schedulerInterface; }
    auto getSchedulerInterface() const -> const Scheduler::Interface & { return *m_schedulerInterface; }
    auto getAudioInterface() -> Audio::Interface & { return *m_audioInterface; }
    auto getAudioInterface() const -> const Audio::Interface & { return *m_audioInterface; }
    auto getVideoInterface() -> Video::Interface & { return *m_videoInterface; }
    auto getVideoInterface() const -> const Video::Interface & { return *m_videoInterface; }
    auto getHapticInterface() -> Haptic::Interface & { return *m_hapticInterface; }
    auto getHapticInterface() const -> const Haptic::Interface & { return *m_hapticInterface; }

    void onStartEvent(unsigned mediaId)
    {
        try
        {
            LOG_INFO("onStartEvent mediaId=", mediaId);
            m_audioInterface->onStartEvent();
            m_videoInterface->onStartEvent();
            m_hapticInterface->onStartEvent();

            m_schedulerInterface->onStartEvent();
            m_decoderInterface->onStartEvent(mediaId);
            m_clientInterface->onStartEvent(mediaId);
        }
        catch (std::exception e)
        {
            LOG_ERROR(e.what());
        }
    }
    void onConfigure(const std::string &configFile)
    {
        try
        {
            LOG_INFO("onConfigure");
            m_clientInterface->onConfigure(configFile);
            m_decoderInterface->onConfigure(configFile);
            m_schedulerInterface->onConfigure(configFile);
            m_audioInterface->onConfigure(configFile);
            m_videoInterface->onConfigure(configFile);
            m_hapticInterface->onConfigure(configFile);
        }
        catch (std::exception e)
        {
            LOG_ERROR(e.what());
        }
    }
    void onPauseEvent(bool b)
    {
        try
        {
            LOG_INFO("onPauseEvent =", b);
            m_audioInterface->onPauseEvent(b);
            m_videoInterface->onPauseEvent(b);
            m_hapticInterface->onPauseEvent(b);
        }
        catch (std::exception e)
        {
            LOG_ERROR(e.what());
        }
    }
    void onStopEvent()
    {
        try
        {
            LOG_INFO("onStopEvent");
            m_audioInterface->onStopEvent();
            m_videoInterface->onStopEvent();
            m_hapticInterface->onStopEvent();

            m_schedulerInterface->onStopEvent();
            m_decoderInterface->onStopEvent();
            m_clientInterface->onStopEvent();
        }
        catch (std::exception e)
        {
            LOG_ERROR(e.what());
        }
    }
    void setOnErrorEventCallback(OnErrorEventCallback ec)
    {
        try
        {
            LOG_INFO("setOnErrorEventCallback");
            m_decoderInterface->setOnErrorEventCallback(ec);
        }
        catch (std::exception e)
        {
            LOG_ERROR(e.what());
        }
    }
    void onMediaRequest(unsigned mediaId)
    {
        try
        {
            LOG_INFO("onMediaRequest mediaId=", mediaId);
            onStopEvent();

            if (mediaId != 0xFF)
            {
                m_decoderInterface->onMediaRequest(mediaId);
                m_clientInterface->onMediaRequest(mediaId);

                onStartEvent(mediaId);
            }
        }
        catch (std::exception e)
        {
            LOG_ERROR(e.what());
        }
    }
};
