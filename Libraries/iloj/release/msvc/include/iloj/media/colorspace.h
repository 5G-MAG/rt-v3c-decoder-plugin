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

#include <iloj/math/matrix.h>
#include <iloj/math/vector.h>
#include <map>

namespace iloj::media
{
class YCC
{
private:
    iloj::math::Mat3x3f m_RGB2YCC = iloj::math::Mat3x3f::eye();
    iloj::math::Mat3x3f m_YCC2RGB = iloj::math::Mat3x3f::eye();

public:
    YCC() = default;
    YCC(const iloj::math::Mat3x3f &RGB2YCC);
    [[nodiscard]] auto getRGBtoYCCMatrix() const -> const iloj::math::Mat3x3f & { return m_RGB2YCC; }
    [[nodiscard]] auto getYCCtoRGBMatrix() const -> const iloj::math::Mat3x3f & { return m_YCC2RGB; }
    static auto BT601() -> const YCC &;
    static auto BT709() -> const YCC &;
    static auto BT2020() -> const YCC &;
};

struct RangeMode
{
    enum
    {
        Limited = 0,
        Full = 1
    };
};

using Chromaticity = iloj::math::Vec2f;

class WhitePoint
{
public:
    static auto D65() -> const Chromaticity &;
};

class ColorSpace
{
private:
    iloj::math::Mat3x3f m_RGB2XYZ = iloj::math::Mat3x3f::eye();
    iloj::math::Mat3x3f m_XYZ2RGB = iloj::math::Mat3x3f::eye();

public:
    ColorSpace() = default;
    ColorSpace(const Chromaticity &r, const Chromaticity &g, const Chromaticity &b, const Chromaticity &w);
    [[nodiscard]] auto getRGBtoXYZMatrix() const -> const iloj::math::Mat3x3f & { return m_RGB2XYZ; }
    [[nodiscard]] auto getXYZtoRGBMatrix() const -> const iloj::math::Mat3x3f & { return m_XYZ2RGB; }
    static auto BT601() -> const ColorSpace &;
    static auto BT709() -> const ColorSpace &;
    static auto BT2020() -> const ColorSpace &;
    static auto sRGB() -> const ColorSpace &;
};

class EOTF
{
private:
    iloj::math::Vec4f m_parameters = {0.F, 1.F, 0.F, 1.F};

public:
    EOTF() = default;
    EOTF(float nu, float alpha, float beta, float gamma): m_parameters({nu, alpha, beta, gamma}) {}
    [[nodiscard]] auto getParameters() const -> const iloj::math::Vec4f & { return m_parameters; }
    static auto Linear() -> const EOTF &;
    static auto BT601() -> const EOTF &;
    static auto BT709() -> const EOTF &;
    static auto BT2020() -> const EOTF &;
    static auto sRGB() -> const EOTF &;
};

class ColorProfile
{
private:
    std::string m_name;
    ColorSpace m_colorSpace;
    EOTF m_eotf;
    YCC m_ycc;
    int m_rangeMode = RangeMode::Full;

public:
    ColorProfile() = default;
    ColorProfile(std::string name, ColorSpace colorSpace, EOTF eotf, YCC ycc = YCC(), int rangeMode = RangeMode::Full)
        : m_name(std::move(name)), m_colorSpace(colorSpace), m_eotf(eotf), m_ycc(ycc), m_rangeMode(rangeMode)
    {
    }
    [[nodiscard]] auto isValid() const -> bool { return (!m_name.empty()); }
    [[nodiscard]] auto getName() const -> const std::string & { return m_name; }
    [[nodiscard]] auto getColorSpace() const -> const ColorSpace & { return m_colorSpace; }
    [[nodiscard]] auto getEOTF() const -> const EOTF & { return m_eotf; }
    [[nodiscard]] auto getYCC() const -> const YCC & { return m_ycc; }
    [[nodiscard]] auto getRangeMode() const -> int { return m_rangeMode; }
    [[nodiscard]] auto previous() const -> const ColorProfile &;
    [[nodiscard]] auto next() const -> const ColorProfile &;
    static auto None() -> const ColorProfile &;
    static auto BT601() -> const ColorProfile &;
    static auto BT601_FullRange() -> const ColorProfile &;
    static auto BT601_Linear() -> const ColorProfile &;
    static auto BT709() -> const ColorProfile &;
    static auto BT709_FullRange() -> const ColorProfile &;
    static auto BT709_Linear() -> const ColorProfile &;
    static auto BT2020() -> const ColorProfile &;
    static auto BT2020_FullRange() -> const ColorProfile &;
    static auto BT2020_Linear() -> const ColorProfile &;
    static auto sRGB() -> const ColorProfile &;
    static auto sRGB_Linear() -> const ColorProfile &;
    static auto fromName(const std::string &name) -> const ColorProfile &;

private:
    static auto getMap() -> std::map<std::string, const ColorProfile *> &;
};
} // namespace iloj::media
