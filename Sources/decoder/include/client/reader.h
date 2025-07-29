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

#include <common/stream/item.h>
#include <iloj/misc/thread.h>
#include <iloj/misc/time.h>
#include <interface/client.h>

// Error codes
enum class LocalCode
{
    LOCAL_OK = 100,     // no error occurred
    LOCAL_INVALID_FILE, // invalid stream file
    LOCAL_UNKNOWN
};

class ReaderInterface: public Client::Interface, public iloj::misc::Service
{
private:
    std::vector<std::string> m_mediaList;
    std::vector<Item> m_itemList;
    std::size_t m_currentItemId{};
    std::size_t m_requestedItemId{0};
    std::chrono::milliseconds m_lookAhead{1000};
    iloj::misc::Timer<std::chrono::system_clock> m_timer;
    iloj::misc::Timer<std::chrono::system_clock>::time_point m_t0;
    std::vector<std::chrono::duration<double>> m_delay{};

    std::chrono::duration<double> m_checkpoint;

public:
    auto getMediaList() -> const std::vector<std::string> & override { return m_mediaList; }
    inline int getMediaId() const override { return m_currentItemId; } 
    void onConfigure(const std::string &configFile) override;
    void onStartEvent(unsigned mediaId) override;
    void onMediaRequest(unsigned mediaId) override;
    void onStopEvent() override;

private:
    void initialize() override;
    void idle() override;
    void finalize() override;

    void updateItem();

    bool m_loop_stream = true;
    bool m_stop = false;
};
