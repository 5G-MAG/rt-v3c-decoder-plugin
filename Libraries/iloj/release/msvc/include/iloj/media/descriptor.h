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

#include "pixelformat.h"
#include <array>
#include <chrono>
#include <iloj/misc/allocator.h>
#include <iloj/misc/memory.h>
#include <sstream>
#include <vector>

namespace iloj::media
{
namespace Descriptor
{
class Data
{
public:
    using container_type = std::vector<std::uint8_t>;

private:
    std::size_t m_capacity{};
    container_type m_frame;
    std::chrono::duration<double> m_timestamp{};

public:
    Data() = default;
    Data(std::size_t capacity, bool allocate = true, const std::chrono::duration<double> &ts = {})
        : m_capacity{capacity}, m_frame(allocate ? capacity : 0), m_timestamp{ts}
    {
    }
    Data(container_type frame, const std::chrono::duration<double> &ts = {})
        : m_capacity{frame.size()}, m_frame{std::move(frame)}, m_timestamp{ts}
    {
    }
    ~Data() = default;
    Data(const Data &other) = default;
    Data(Data &&other) noexcept
        : m_capacity{other.m_capacity}, m_frame(std::move(other.m_frame)), m_timestamp{other.m_timestamp}
    {
        other.m_capacity = {};
        other.m_timestamp = {};
    }
    auto operator=(const Data &other) -> Data & = default;
    auto operator=(Data &&other) noexcept -> Data &
    {
        m_capacity = other.m_capacity;
        m_frame = std::move(other.m_frame);
        m_timestamp = other.m_timestamp;

        other.m_capacity = {};
        other.m_timestamp = {};

        return *this;
    }
    void allocate(std::size_t capacity)
    {
        m_capacity = capacity;
        m_frame.resize(capacity);
    }
    template<typename T>
    void set(const T &value)
    {
        allocate(sizeof(T));
        *reinterpret_cast<T *>(m_frame.data()) = value;
    }
    template<typename T>
    auto get() const -> const T &
    {
        return *reinterpret_cast<const T *>(m_frame.data());
    }
    [[nodiscard]] auto isAllocated() const -> bool { return !m_frame.empty(); }
    [[nodiscard]] auto getCapacity() const -> std::size_t { return m_capacity; }
    auto getFrame() -> container_type & { return m_frame; }
    [[nodiscard]] auto getFrame() const -> const container_type & { return m_frame; }
    [[nodiscard]] auto getTimeStamp() const -> const std::chrono::duration<double> & { return m_timestamp; }
    void setTimeStamp(const std::chrono::duration<double> &v) { m_timestamp = v; }
    void log(std::ostream &os) const;
    void dump(std::ostream &os) const;
    auto dump(const std::string &path) const -> bool;
    void read(std::istream &is);
    auto read(const std::string &path) -> bool;
};

class Audio
{
public:
    enum class FormatId
    {
        None = 0,
        U8,
        S16,
        S32,
        FLT
    };
    enum class PackingId
    {
        None = 0,
        Interleaved,
        Planar
    };
    using container_type = std::vector<std::uint8_t>;

public:
    FormatId m_format = FormatId::None;
    PackingId m_packing = PackingId::None;
    unsigned m_nbChannels{};
    unsigned m_rate{};
    container_type m_buffer;
    std::vector<std::uint8_t *> m_frame;
    unsigned m_bytePerChannel{};
    Data m_metaData;

public:
    Audio() = default;
    Audio(FormatId format,
          PackingId packing,
          unsigned nbChannels,
          unsigned rate,
          unsigned nbSamples,
          bool allocate = true);
    ~Audio() = default;
    Audio(const Audio &other) = default;
    Audio(Audio &&other) noexcept
    {
        m_format = other.m_format;
        m_packing = other.m_packing;
        m_nbChannels = other.m_nbChannels;
        m_rate = other.m_rate;
        m_buffer = std::move(other.m_buffer);
        m_frame = std::move(other.m_frame);
        m_bytePerChannel = other.m_bytePerChannel;
        m_metaData = std::move(other.m_metaData);

        other.m_format = FormatId::None;
        other.m_packing = PackingId::None;
        other.m_nbChannels = {};
        other.m_rate = {};
        other.m_bytePerChannel = {};
    }
    auto operator=(const Audio &other) -> Audio & = default;
    auto operator=(Audio &&other) noexcept -> Audio &
    {
        m_format = other.m_format;
        m_packing = other.m_packing;
        m_nbChannels = other.m_nbChannels;
        m_rate = other.m_rate;
        m_buffer = std::move(other.m_buffer);
        m_frame = std::move(other.m_frame);
        m_bytePerChannel = other.m_bytePerChannel;
        m_metaData = std::move(other.m_metaData);

        other.m_format = FormatId::None;
        other.m_packing = PackingId::None;
        other.m_nbChannels = {};
        other.m_rate = {};
        other.m_bytePerChannel = {};

        return *this;
    }
    void resize(unsigned nbSamples, bool allocate);
    [[nodiscard]] auto isValid() const -> bool { return (m_format != FormatId::None); }
    [[nodiscard]] auto isAllocated() const -> bool { return !m_buffer.empty(); }
    [[nodiscard]] auto getFrame() const -> const std::vector<std::uint8_t *> & { return m_frame; }
    [[nodiscard]] auto getFormat() const -> FormatId { return m_format; }
    [[nodiscard]] auto getPacking() const -> PackingId { return m_packing; }
    [[nodiscard]] auto getNumberOfChannels() const -> unsigned { return m_nbChannels; }
    [[nodiscard]] auto getRate() const -> unsigned { return m_rate; }
    [[nodiscard]] auto getBytePerChannel() const -> unsigned { return m_bytePerChannel; }
    [[nodiscard]] auto getSamplePerChannel() const -> unsigned;
    [[nodiscard]] auto getBytePerFrame() const -> unsigned { return (m_bytePerChannel * m_nbChannels); }
    auto getMetadata() -> Data & { return m_metaData; }
    [[nodiscard]] auto getMetadata() const -> const Data & { return m_metaData; }
    void log(std::ostream &os) const;
    static auto getStringFromFormat(FormatId id) -> std::string;
    static auto getStringFromPacking(PackingId id) -> std::string;
};

class Video
{
public:
    using container_type = std::vector<std::uint8_t, iloj::misc::Allocator::Aligned<std::uint8_t>>;
    using HardwareContext = std::vector<std::uint8_t>;

public:
    template<std::size_t SZ0, std::size_t SZ1, std::size_t SZ2, std::size_t SZ3>
    struct Stacked
    {
    public:
        int m_pixelFormatId{};
        unsigned m_width{}, m_height{};
        std::array<std::uint8_t, SZ0> m_plane_0;
        std::array<std::uint8_t, SZ1> m_plane_1;
        std::array<std::uint8_t, SZ2> m_plane_2;
        std::array<std::uint8_t, SZ3> m_plane_3;
    };

public:
    std::unique_ptr<PixelFormat::Base> m_pixelFormat;
    unsigned m_width{}, m_height{};
    container_type m_buffer;
    std::array<std::uint8_t *, 4> m_frame{};
    std::array<int, 4> m_lineSize{};
    HardwareContext m_hwContext;
    Data m_metaData;

public:
    Video() = default;
    Video(std::unique_ptr<PixelFormat::Base> pixelFormat,
          unsigned width,
          unsigned height,
          unsigned alignment = 4,
          bool allocate = true);
    Video(unsigned pixelFormatId, unsigned width, unsigned height, unsigned alignment = 4, bool allocate = true)
        : Video(PixelFormat::fromId(pixelFormatId), width, height, alignment, allocate)
    {
    }
    Video(const std::string &pixelFormatName,
          unsigned width,
          unsigned height,
          unsigned alignment = 4,
          bool allocate = true)
        : Video(PixelFormat::fromName(pixelFormatName), width, height, alignment, allocate)
    {
    }
    ~Video() = default;
    Video(const Video &other) = delete;
    Video(Video &&other) noexcept
    {
        m_pixelFormat = std::move(other.m_pixelFormat);
        m_width = other.m_width;
        m_height = other.m_height;
        m_buffer = std::move(other.m_buffer);
        m_frame = other.m_frame;
        m_lineSize = other.m_lineSize;
        m_hwContext = std::move(other.m_hwContext);
        m_metaData = std::move(other.m_metaData);

        other.m_width = {};
        other.m_height = {};
        other.m_frame = {};
        other.m_lineSize = {};
    }
    auto operator=(const Video &) -> Video & = delete;
    auto operator=(Video &&other) noexcept -> Video &
    {
        m_pixelFormat = std::move(other.m_pixelFormat);
        m_width = other.m_width;
        m_height = other.m_height;
        m_buffer = std::move(other.m_buffer);
        m_frame = other.m_frame;
        m_lineSize = other.m_lineSize;
        m_hwContext = std::move(other.m_hwContext);
        m_metaData = std::move(other.m_metaData);

        other.m_width = {};
        other.m_height = {};
        other.m_frame = {};
        other.m_lineSize = {};

        return *this;
    }
    template<typename PIXELFORMAT>
    static auto create(unsigned width, unsigned height, unsigned alignment = 4, bool allocate = true) -> Video
    {
        return Video(std::make_unique<PIXELFORMAT>(), width, height, alignment, allocate);
    }
    [[nodiscard]] auto isValid() const -> bool { return (m_pixelFormat != nullptr); }
    [[nodiscard]] auto isAllocated() const -> bool { return !m_buffer.empty(); }
    [[nodiscard]] auto getPixelFormat() const -> const PixelFormat::Base & { return *m_pixelFormat; }
    [[nodiscard]] auto getWidth() const -> unsigned { return m_width; }
    [[nodiscard]] auto getHeight() const -> unsigned { return m_height; }
    [[nodiscard]] auto getFrame() const -> const std::array<std::uint8_t *, 4> & { return m_frame; }
    [[nodiscard]] auto getLineSize() const -> const std::array<int, 4> & { return m_lineSize; }
    [[nodiscard]] auto getWidth(unsigned plane) const -> unsigned { return m_pixelFormat->getWidth(plane, m_width); }
    [[nodiscard]] auto getHeight(unsigned plane) const -> unsigned { return m_pixelFormat->getHeight(plane, m_height); }
    [[nodiscard]] auto getPixelPerLine(unsigned plane) const -> unsigned
    {
        return (unsigned) m_lineSize[plane] / m_pixelFormat->getBytePerSample(plane);
    }
    [[nodiscard]] auto getBytePerRow(unsigned plane) const -> unsigned
    {
        return m_pixelFormat->getBytePerSample(plane) * getWidth(plane);
    }
    [[nodiscard]] auto getBytePerPlane(unsigned plane) const -> unsigned
    {
        return (unsigned) m_lineSize[plane] * getHeight(plane);
    }
    [[nodiscard]] auto getBytePerPlaneOnDisk(unsigned plane) const -> unsigned
    {
        return getBytePerRow(plane) * getHeight(plane);
    }
    [[nodiscard]] auto getBytePerFrame() const -> unsigned
    {
        unsigned bytes = 0;
        for (unsigned i = 0; i < m_pixelFormat->getNumberOfPlane(); i++)
        {
            bytes += getBytePerPlane(i);
        }
        return bytes;
    }
    [[nodiscard]] auto getBytePerFrameOnDisk() const -> unsigned
    {
        return m_pixelFormat->getBytePerFrame(m_width, m_height, 1);
    }
    auto getHardwareContext() -> HardwareContext & { return m_hwContext; }
    [[nodiscard]] auto getHardwareContext() const -> const HardwareContext & { return m_hwContext; }
    auto getMetadata() -> Data & { return m_metaData; }
    [[nodiscard]] auto getMetadata() const -> const Data & { return m_metaData; }
    void log(std::ostream &os) const;
    void dump(std::ostream &os, bool vflip = false) const;
    auto dump(const std::string &path, bool vflip = false) const -> bool;
    void read(std::istream &is, bool vflip = false);
    auto read(const std::string &path, unsigned frame = 0, bool vflip = false) -> bool;
    void toStacked(std::ostream &os, const std::string &name) const;
    template<std::size_t SZ0, std::size_t SZ1, std::size_t SZ2, std::size_t SZ3>
    static auto fromStacked(const Stacked<SZ0, SZ1, SZ2, SZ3> &stacked) -> Video
    {
        Video out(stacked.m_pixelFormatId, stacked.m_width, stacked.m_height, 1U);

        std::copy(stacked.m_plane_0.begin(), stacked.m_plane_0.end(), out.getFrame()[0]);
        std::copy(stacked.m_plane_1.begin(), stacked.m_plane_1.end(), out.getFrame()[1]);
        std::copy(stacked.m_plane_2.begin(), stacked.m_plane_2.end(), out.getFrame()[2]);
        std::copy(stacked.m_plane_3.begin(), stacked.m_plane_3.end(), out.getFrame()[3]);

        return out;
    }
};
} // namespace Descriptor

auto loadPPM(const std::string &path) -> Descriptor::Video;
auto loadPGM(const std::string &path) -> Descriptor::Video;
auto savePPM(const Descriptor::Video &desc, const std::string &path) -> bool;
auto savePGM(const Descriptor::Video &desc, const std::string &path) -> bool;
} // namespace iloj::media
