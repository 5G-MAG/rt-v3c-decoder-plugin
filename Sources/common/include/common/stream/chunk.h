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

#include <chrono>
#include <vector>

class Chunk
{
public:
    class Header
    {
    public:
        enum TypeId : std::uint8_t
        {
            None = 0,
            Audio,
            Hevc,
            Vvc,
            Miv,
            Vpcc,
            Haptic
        };

    private:
        std::uint8_t m_typeId{};
        std::uint16_t m_mediaId{};
        std::uint16_t m_segmentId{};
        double m_pts{};
        double m_duration{};
        unsigned int m_errorStreamer{};
        std::size_t m_dataSize{};
        std::uint32_t m_nbFrame{};
        unsigned int m_seqNumber{};
        long long m_timestampDbg{ 0 };
        double m_segmentDuration{ 0.0 };

    public:
        Header() = default;
        Header(const Header &) = default;
        Header(Header &&) = default;
        auto operator=(const Header &) -> Header & = default;
        auto operator=(Header &&) -> Header & = default;
        void setTypeId(int typeId) { m_typeId = typeId; }
        auto getTypeId() const -> int { return m_typeId; }
        void setPTS(const std::chrono::duration<double> &pts) { m_pts = pts.count(); }
        auto getPTS() const -> std::chrono::duration<double> { return std::chrono::duration<double>{m_pts}; }
        void setDuration(const std::chrono::duration<double> &duration) { m_duration = duration.count(); }
        auto getDuration() const -> std::chrono::duration<double> { return std::chrono::duration<double>{m_duration}; }
        void setErrorStreamer(unsigned int errorStreamer) { m_errorStreamer = errorStreamer; }
        auto getErrorStreamer() const -> unsigned int { return m_errorStreamer; }
        void setMediaId(std::uint16_t mediaId) { m_mediaId = mediaId; }
        auto getMediaId() const -> std::uint16_t { return m_mediaId; }
        void setSegmentId(std::uint16_t segmentId) { m_segmentId = segmentId; }
        auto getSegmentId() const -> std::uint16_t { return m_segmentId; }
        void setDataSize(std::size_t sz) { m_dataSize = sz; }
        auto getDataSize() const -> std::size_t { return m_dataSize; }
        void setNumberOfFrames(std::uint32_t nbFrame) { m_nbFrame = nbFrame; }
        auto getNumberOfFrames() const -> std::uint32_t { return m_nbFrame; }
        void setSeqNumber(std::uint32_t seqNum) { m_seqNumber = seqNum; }
        auto getSeqNumber() const -> std::uint32_t { return m_seqNumber; }
        void setSegmentDuration(double segDur) { m_segmentDuration = segDur; }
        auto getSegmentDuration() const -> double { return m_segmentDuration; }
        void setTimestampDbg(long long timestampDbg) { m_timestampDbg = timestampDbg; }
        auto getTimestampDbg() const -> long long { return m_timestampDbg; }
    };

    using Buffer = std::vector<std::uint8_t>;

private:
    Header m_header{};
    Buffer m_data{};

public:
    Chunk() = default;
    Chunk(const Header &header, Buffer data): m_header{header}, m_data{std::move(data)}
    {
        m_header.setDataSize(m_data.size());
    }
    Chunk(const Chunk &) = default;
    Chunk(Chunk &&) = default;
    auto operator=(const Chunk &) -> Chunk & = default;
    auto operator=(Chunk &&) -> Chunk & = default;
    auto getHeader() -> Header & { return m_header; }
    auto getHeader() const -> const Header & { return m_header; }
    void setData(Buffer&& data)
    {
        m_data = std::move(data);
        m_header.setDataSize(m_data.size());
    }
    auto getData() const -> const Buffer & { return m_data; }
    auto getData() -> Buffer & { return m_data; }
};
