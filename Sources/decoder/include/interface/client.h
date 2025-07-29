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

#include "decoder.h"

namespace Client
{
class Interface
{
protected:
    Decoder::Interface *m_decoderInterface = nullptr;

public:
    Interface(){};
    virtual ~Interface() {}
    Interface(const Interface &other) = delete;
    Interface(Interface &&other) noexcept = default;
    auto operator=(const Interface &) -> Interface & = delete;
    auto operator=(Interface &&other) noexcept -> Interface & = default;
    void setDecoderInterface(Decoder::Interface *decoderInterface) { m_decoderInterface = decoderInterface; }
    virtual auto getMediaList() -> const std::vector<std::string> & = 0;
    virtual int getMediaId() const = 0;
    virtual void onConfigure(const std::string &configFile) = 0;
    virtual void onStartEvent(unsigned mediaId) = 0;
    virtual void onMediaRequest(unsigned mediaId) = 0;
    virtual void onStopEvent() = 0;
};

} // namespace Client
