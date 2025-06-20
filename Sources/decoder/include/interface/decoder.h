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

#include "scheduler.h"
#include <common/stream/chunk.h>

typedef void(*OnErrorEventCallback)(unsigned int, unsigned int);

namespace Decoder
{
class Interface
{
protected:
    Scheduler::Interface *m_schedulerInterface = nullptr;
    OnErrorEventCallback m_onErrorEventCallback = nullptr;

public:
    Interface(){};
    virtual ~Interface() {}
    Interface(const Interface &other) = delete;
    Interface(Interface &&other) noexcept = default;
    auto operator=(const Interface &) -> Interface & = delete;
    auto operator=(Interface &&other) noexcept -> Interface & = default;
    void setSchedulerInterface(Scheduler::Interface *schedulerInterface) { m_schedulerInterface = schedulerInterface; }
    virtual void onConfigure(const std::string &configFile) = 0;
    virtual void setSharedOpenGLContext(HANDLE hwContext) = 0;
    virtual void onStartEvent(unsigned mediaId) = 0;
    virtual void onChunkEvent(Chunk&& chunk) = 0;
    virtual void onStopEvent() = 0;
    virtual void setOnErrorEventCallback(OnErrorEventCallback ec) { m_onErrorEventCallback = ec; }
    virtual auto getOnErrorEventCallback() -> OnErrorEventCallback { return m_onErrorEventCallback; }
    virtual void onMediaRequest(unsigned mediaId) = 0;
    virtual double getDecoderFPS()  = 0;
    virtual void flushFPSMeasures()  = 0;
    virtual int getAtlasFrameHeight() = 0;
    virtual int getAtlasFrameWidth() = 0;
    
};

} // namespace Decoder