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

#include <interface/client.h>
#include <common/stream/item.h>

class MetaInterface: public Client::Interface
{
private:
#if defined DASH_STREAMING || defined UVG_RTP_STREAMING || defined WEBRTC_RTP_STREAMING
    std::unique_ptr < Client::Interface> m_realInterface[2];
#else
    std::unique_ptr<Client::Interface> m_realInterface;
#endif

    std::vector<Item> m_itemList;

public:
    auto getMediaList() -> const std::vector<std::string> & override;
    int getMediaId() const override;
    void onConfigure(const std::string &configFile) override;
    void onStartEvent(unsigned mediaId) override;
    void onMediaRequest(unsigned mediaId) override;
    void onStopEvent() override;
};
