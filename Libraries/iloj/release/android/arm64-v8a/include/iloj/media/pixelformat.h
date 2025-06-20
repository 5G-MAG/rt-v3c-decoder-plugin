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

#include <cstdlib>
#include <iloj/math/essentials.h>
#include <iloj/misc/string.h>

namespace iloj::media
{
namespace PixelFormat
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct Id
{
    enum
    {
        NONE,
        RGB8,
        RGBA8,
        RGB16LE,
        RGBA16LE,
        BGR8,
        YUV400P8,
        YUV400P10LE,
        YUV400P12LE,
        YUV400P16LE,
        YUV400P32LE,
        YUV420P8,
        YUV420P10LE,
        YUV420P12LE,
        YUV420P16LE,
        YUV422P8,
        YUV422P10LE,
        YUV422P12LE,
        YUV422P16LE,
        YUV444P8,
        YUV444P10LE,
        YUV444P12LE,
        YUV444P16LE,
        NV8,
        NV10,
        MEDIACODEC,
        MEDIACODEC_RGB,
        MEDIACODEC_YUV
    };
};

template<unsigned BITPERSAMPLE>
struct BaseType
{
};

template<>
struct BaseType<8>
{
    using value_type = std::uint8_t;
};

template<>
struct BaseType<10>
{
    using value_type = std::uint16_t;
};

template<>
struct BaseType<12>
{
    using value_type = std::uint16_t;
};

template<>
struct BaseType<16>
{
    using value_type = std::uint16_t;
};

template<>
struct BaseType<32>
{
    using value_type = std::uint32_t;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Base
{
public:
    Base() = default;
    virtual ~Base() = default;
    Base(const Base &) = default;
    Base(Base &&) = default;
    auto operator=(const Base &) -> Base & = default;
    auto operator=(Base &&) -> Base & = default;
    [[nodiscard]] virtual auto getNumberOfPlane() const -> unsigned = 0;
    [[nodiscard]] virtual auto getBytePerSample(unsigned plane) const -> unsigned = 0;
    [[nodiscard]] virtual auto getBitDepth() const -> unsigned = 0;
    [[nodiscard]] virtual auto getWidth(unsigned plane, unsigned width) const -> unsigned = 0;
    [[nodiscard]] virtual auto getHeight(unsigned plane, unsigned height) const -> unsigned = 0;
    [[nodiscard]] virtual auto hasColor() const -> bool = 0;
    [[nodiscard]] virtual auto getLuminancePlane() const -> int = 0;
    [[nodiscard]] virtual auto isBigEndian() const -> bool = 0;
    [[nodiscard]] virtual auto getName() const -> std::string = 0;
    [[nodiscard]] virtual auto getId() const -> unsigned = 0;
    [[nodiscard]] auto getBytePerRow(unsigned plane, unsigned width) const -> unsigned
    {
        return getBytePerSample(plane) * getWidth(plane, width);
    }
    [[nodiscard]] auto getLineSize(unsigned plane, unsigned width, unsigned alignment = 1) const -> unsigned
    {
        auto granularity = iloj::math::lcm(getBytePerSample(plane), alignment);
        auto q = std::div(static_cast<int>(getBytePerRow(plane, width)), static_cast<int>(granularity));
        return (granularity * (unsigned) (q.quot + ((q.rem != 0) ? 1 : 0)));
    }
    [[nodiscard]] auto getPixelPerLine(unsigned plane, unsigned width, unsigned alignment = 1) const -> unsigned
    {
        return (unsigned) getLineSize(plane, width, alignment) / getBytePerSample(plane);
    }
    [[nodiscard]] auto getBytePerPlane(unsigned plane, unsigned width, unsigned height, unsigned alignment = 1) const
        -> unsigned
    {
        return (unsigned) getLineSize(plane, width, alignment) * getHeight(plane, height);
    }
    [[nodiscard]] auto getBytePerFrame(unsigned width, unsigned height, unsigned alignment = 1) const -> unsigned
    {
        unsigned bytes{};
        for (unsigned plane = 0U; plane < getNumberOfPlane(); plane++)
        {
            bytes += getBytePerPlane(plane, width, height, alignment);
        }
        return bytes;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<unsigned ID, bool ALPHA, bool MIRROR, unsigned BITPERSAMPLE, bool ENDIANESS>
class RGB: public Base
{
public:
    using base_type = typename BaseType<BITPERSAMPLE>::value_type;
    static constexpr unsigned Id = ID;

public:
    [[nodiscard]] auto getNumberOfPlane() const -> unsigned override { return 1; }
    [[nodiscard]] auto getBytePerSample(unsigned plane) const -> unsigned override
    {
        if (plane < 1U)
        {
            auto q = div(static_cast<int>(BITPERSAMPLE), 8);
            return (ALPHA ? 4U : 3U) * static_cast<unsigned>(q.quot + ((q.rem != 0) ? 1 : 0));
        }

        return 0;
    }
    [[nodiscard]] auto getBitDepth() const -> unsigned override { return BITPERSAMPLE; }
    [[nodiscard]] auto getWidth(unsigned plane, unsigned width) const -> unsigned override
    {
        return (plane < 1) ? width : 0;
    }
    [[nodiscard]] auto getHeight(unsigned plane, unsigned height) const -> unsigned override
    {
        return (plane < 1) ? height : 0;
    }
    [[nodiscard]] auto hasColor() const -> bool override { return true; }
    [[nodiscard]] auto getLuminancePlane() const -> int override { return -1; }
    [[nodiscard]] auto isBigEndian() const -> bool override { return ENDIANESS; }
    [[nodiscard]] auto getName() const -> std::string override
    {
        using namespace std::string_literals;
        return iloj::misc::any2str(MIRROR ? "bgr"s : "rgb"s,
                                   ALPHA ? "a"s : "",
                                   BITPERSAMPLE,
                                   (8 < BITPERSAMPLE) ? (ENDIANESS ? "be"s : "le"s) : "");
    }
    [[nodiscard]] auto getId() const -> unsigned override { return ID; }
    static auto getBaseType() -> base_type {}
};

using RGB8 = RGB<Id::RGB8, false, false, 8, false>;
using RGBA8 = RGB<Id::RGBA8, true, false, 8, false>;
using RGB16LE = RGB<Id::RGB16LE, false, false, 16, false>;
using RGBA16LE = RGB<Id::RGBA16LE, true, false, 16, false>;
using BGR8 = RGB<Id::BGR8, false, true, 8, false>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<unsigned ID, unsigned A, unsigned B, unsigned BITPERSAMPLE, bool ENDIANESS>
class YUVP: public Base
{
public:
    using base_type = typename BaseType<BITPERSAMPLE>::value_type;
    static constexpr unsigned Id = ID;

public:
    [[nodiscard]] auto getNumberOfPlane() const -> unsigned override { return (A == 0U) ? 1 : 3; }
    [[nodiscard]] auto getBytePerSample(unsigned plane) const -> unsigned override
    {
        if (plane < getNumberOfPlane())
        {
            auto q = div(static_cast<int>(BITPERSAMPLE), 8);
            return static_cast<unsigned>(q.quot + ((q.rem != 0) ? 1 : 0));
        }

        return 0;
    }
    [[nodiscard]] auto getBitDepth() const -> unsigned override { return BITPERSAMPLE; }
    [[nodiscard]] auto getWidth(unsigned plane, unsigned width) const -> unsigned override
    {
        if (plane < getNumberOfPlane())
        {
            return (plane == 0) ? width : ((A * width) / 4);
        }

        return 0;
    }
    [[nodiscard]] auto getHeight(unsigned plane, unsigned height) const -> unsigned override
    {
        if (plane < getNumberOfPlane())
        {
            return (plane == 0) ? height : ((A != B) ? (height / 2) : height);
        }

        return 0;
    }
    [[nodiscard]] auto hasColor() const -> bool override { return (A != 0); }
    [[nodiscard]] auto getLuminancePlane() const -> int override { return 0; }
    [[nodiscard]] auto isBigEndian() const -> bool override { return ENDIANESS; }
    [[nodiscard]] auto getName() const -> std::string override
    {
        using namespace std::string_literals;

        return iloj::misc::any2str(
            "yuv4", A, B, 'p', BITPERSAMPLE, (8 < BITPERSAMPLE) ? (ENDIANESS ? "be"s : "le"s) : "");
    }
    [[nodiscard]] auto getId() const -> unsigned override { return ID; }
    static auto getBaseType() -> base_type {}
};

using YUV400P8 = YUVP<Id::YUV400P8, 0, 0, 8, false>;
using YUV400P10LE = YUVP<Id::YUV400P10LE, 0, 0, 10, false>;
using YUV400P12LE = YUVP<Id::YUV400P12LE, 0, 0, 12, false>;
using YUV400P16LE = YUVP<Id::YUV400P16LE, 0, 0, 16, false>;
using YUV400P32LE = YUVP<Id::YUV400P32LE, 0, 0, 32, false>;

using YUV420P8 = YUVP<Id::YUV420P8, 2, 0, 8, false>;
using YUV420P10LE = YUVP<Id::YUV420P10LE, 2, 0, 10, false>;
using YUV420P12LE = YUVP<Id::YUV420P12LE, 2, 0, 12, false>;
using YUV420P16LE = YUVP<Id::YUV420P16LE, 2, 0, 16, false>;

using YUV422P8 = YUVP<Id::YUV422P8, 2, 2, 8, false>;
using YUV422P10LE = YUVP<Id::YUV422P10LE, 2, 2, 10, false>;
using YUV422P12LE = YUVP<Id::YUV422P12LE, 2, 2, 12, false>;
using YUV422P16LE = YUVP<Id::YUV422P16LE, 2, 2, 16, false>;

using YUV444P8 = YUVP<Id::YUV444P8, 4, 4, 8, false>;
using YUV444P10LE = YUVP<Id::YUV444P10LE, 4, 4, 10, false>;
using YUV444P12LE = YUVP<Id::YUV444P12LE, 4, 4, 12, false>;
using YUV444P16LE = YUVP<Id::YUV444P16LE, 4, 4, 16, false>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<unsigned ID, unsigned BITPERSAMPLE>
class NV: public Base
{
public:
    using base_type = typename BaseType<BITPERSAMPLE>::value_type;
    static constexpr unsigned Id = ID;

public:
    [[nodiscard]] auto getNumberOfPlane() const -> unsigned override { return 2; }
    [[nodiscard]] auto getBytePerSample(unsigned plane) const -> unsigned override
    {
        if (plane < 2U)
        {
            auto q = std::div((int) BITPERSAMPLE, 8);
            auto bps = (unsigned) (q.quot + ((q.rem != 0) ? 1 : 0));
            return (plane == 0) ? bps : (2 * bps);
        }

        return 0;
    }
    [[nodiscard]] auto getBitDepth() const -> unsigned override { return BITPERSAMPLE; }
    [[nodiscard]] auto getWidth(unsigned plane, unsigned width) const -> unsigned override
    {
        if (plane < 2U)
        {
            return (plane == 0) ? width : (width / 2);
        }

        return 0;
    }
    [[nodiscard]] auto getHeight(unsigned plane, unsigned height) const -> unsigned override
    {
        if (plane < 2U)
        {
            return (plane == 0) ? height : (height / 2);
        }

        return 0;
    }
    [[nodiscard]] auto hasColor() const -> bool override { return true; }
    [[nodiscard]] auto getLuminancePlane() const -> int override { return 0; }
    [[nodiscard]] auto isBigEndian() const -> bool override { return false; }
    [[nodiscard]] auto getName() const -> std::string override { return iloj::misc::any2str("nv", BITPERSAMPLE); }
    [[nodiscard]] auto getId() const -> unsigned override { return ID; }
    static auto getBaseType() -> base_type {}
};

using NV8 = NV<Id::NV8, 8>;
using NV10 = NV<Id::NV10, 10>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MEDIACODEC: public Base
{
public:
    using base_type = std::uint8_t;
    static constexpr unsigned Id = PixelFormat::Id::MEDIACODEC;

public:
    [[nodiscard]] auto getNumberOfPlane() const -> unsigned override { return 0; }
    [[nodiscard]] auto getBytePerSample(unsigned /* plane */) const -> unsigned override { return 0; }
    [[nodiscard]] auto getBitDepth() const -> unsigned override { return 0; }
    [[nodiscard]] auto getWidth(unsigned /* plane */, unsigned /* width */) const -> unsigned override { return 0; }
    [[nodiscard]] auto getHeight(unsigned /* plane */, unsigned /* height */) const -> unsigned override { return 0; }
    [[nodiscard]] auto hasColor() const -> bool override { return true; }
    [[nodiscard]] auto getLuminancePlane() const -> int override { return -1; }
    [[nodiscard]] auto isBigEndian() const -> bool override { return false; }
    [[nodiscard]] auto getName() const -> std::string override { return "mediacodec"; }
    [[nodiscard]] auto getId() const -> unsigned override { return Id::MEDIACODEC; }
    static auto getBaseType() -> base_type { return 0; }
};

class MEDIACODEC_YUV: public Base
{
public:
    using base_type = std::uint8_t;
    static constexpr unsigned Id = PixelFormat::Id::MEDIACODEC_YUV;

public:
    [[nodiscard]] auto getNumberOfPlane() const -> unsigned override { return 0; }
    [[nodiscard]] auto getBytePerSample(unsigned /* plane */) const -> unsigned override { return 0; }
    [[nodiscard]] auto getBitDepth() const -> unsigned override { return 0; }
    [[nodiscard]] auto getWidth(unsigned /* plane */, unsigned /* width */) const -> unsigned override { return 0; }
    [[nodiscard]] auto getHeight(unsigned /* plane */, unsigned /* height */) const -> unsigned override { return 0; }
    [[nodiscard]] auto hasColor() const -> bool override { return true; }
    [[nodiscard]] auto getLuminancePlane() const -> int override { return -1; }
    [[nodiscard]] auto isBigEndian() const -> bool override { return false; }
    [[nodiscard]] auto getName() const -> std::string override { return "mediacodec_yuv"; }
    [[nodiscard]] auto getId() const -> unsigned override { return Id::MEDIACODEC_YUV; }
    static auto getBaseType() -> base_type { return 0; }
};

class MEDIACODEC_RGB: public Base
{
public:
    using base_type = std::uint8_t;
    static constexpr unsigned Id = PixelFormat::Id::MEDIACODEC_RGB;

public:
    [[nodiscard]] auto getNumberOfPlane() const -> unsigned override { return 0; }
    [[nodiscard]] auto getBytePerSample(unsigned /* plane */) const -> unsigned override { return 0; }
    [[nodiscard]] auto getBitDepth() const -> unsigned override { return 0; }
    [[nodiscard]] auto getWidth(unsigned /* plane */, unsigned /* width */) const -> unsigned override { return 0; }
    [[nodiscard]] auto getHeight(unsigned /* plane */, unsigned /* height */) const -> unsigned override { return 0; }
    [[nodiscard]] auto hasColor() const -> bool override { return true; }
    [[nodiscard]] auto getLuminancePlane() const -> int override { return -1; }
    [[nodiscard]] auto isBigEndian() const -> bool override { return false; }
    [[nodiscard]] auto getName() const -> std::string override { return "mediacodec_rgb"; }
    [[nodiscard]] auto getId() const -> unsigned override { return Id::MEDIACODEC_RGB; }
    static auto getBaseType() -> base_type { return 0; }
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
auto fromId(unsigned id) -> std::unique_ptr<Base>;
auto fromName(const std::string &s) -> std::unique_ptr<Base>;
} // namespace PixelFormat
} // namespace iloj::media
