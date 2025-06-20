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

#include "chunk.h"
#include <iloj/misc/json.h>

class Item
{
public:
    class Stream
    {
    private:
        std::uint8_t m_typeId{};
        std::uint16_t m_nbSegment{};
        std::string m_path{};
        std::string m_url{};
        std::string m_serverName{};
        double m_framerate{};
        // for RTP
        std::string m_senderName{};

    public:
        Stream(std::uint8_t typeId,
               std::uint16_t nbSegment,
               std::string path,
               std::string url,
               std::string serverName,
               std::string senderName,
               double framerate = 25.0
            )
            : m_typeId{typeId},
              m_nbSegment{nbSegment},
              m_path{std::move(path)},
              m_url{std::move(url)},
              m_serverName{std::move(serverName)},
              m_senderName{ std::move(senderName) },
              m_framerate{ framerate }
        {
        }
        Stream(const Stream &) = default;
        Stream(Stream &&) = default;
        auto operator=(const Stream &) -> Stream & = default;
        auto operator=(Stream &&) -> Stream & = default;
        auto getTypeId() const -> std::uint8_t { return m_typeId; }
        auto getNumberOfSegments() const -> std::uint16_t { return m_nbSegment; }
        auto getPath() const -> std::string { return m_path; }
        inline auto getFramerate() const -> double { return m_framerate; }
        inline auto getUrl() const -> std::string { return m_url; }
        inline auto getUrlRef() const -> const std::string & { return m_url; }
        inline auto getServerName() const -> std::string { return m_serverName; }
        inline auto getServerNameRef() const -> const std::string & { return m_serverName; }
        inline auto getSenderName() const -> std::string { return m_senderName; }
    };

    class State
    {
    private:
        int m_segmentId{};
        double m_streamDelay{};

    public:
        State() = default;
        State(int segmentId, double streamDelay): m_segmentId{segmentId}, m_streamDelay{streamDelay} {}
        State(const State &) = default;
        State(State &&) = default;
        auto operator=(const State &) -> State & = default;
        auto operator=(State &&) -> State & = default;
        auto getSegmentId() const -> int { return m_segmentId; }
        auto getStreamDelay() const -> double { return m_streamDelay; }
        void update(double duration, int nbSegment)
        {
            m_segmentId++;
            m_streamDelay += duration;

            if (nbSegment <= m_segmentId)
            {
                m_segmentId = -1;
            }
        }
    };

    using Property = std::vector<std::pair<double, std::uint32_t>>;

private:
    int m_itemId{-1};
    std::string m_name;
    std::vector<Stream> m_streamList{};
    std::vector<State> m_streamState{};
    std::vector<Property> m_streamProperty{};
    std::string m_mode;

public:
    Item() = default;
    Item(iloj::misc::JSON::Object &config, int itemId, bool buildIndex = false);
    Item(const Item &) = default;
    Item(Item &&) = default;
    auto operator=(const Item &) -> Item & = default;
    auto operator=(Item &&) -> Item & = default;
    auto getItemId() const -> int { return m_itemId; }
    auto getName() const -> const std::string & { return m_name; }
    auto getMode() const -> const std::string & { return m_mode; }
    void reset();
    auto next() -> std::tuple<std::size_t, Chunk, std::chrono::duration<double>>;
    auto getNumberOfStreams() const -> std::size_t { return m_streamList.size(); }

    static std::vector<Item> makeItemList(iloj::misc::JSON::Object &config, unsigned int nbChannel, bool buildIndex = false);
    inline const std::vector<Stream> &getStreams() const { return m_streamList; }
};