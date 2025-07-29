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

#include "interactor.h"
#include "plan.h"
#include <iloj/media/colorspace.h>
#include <iloj/media/descriptor.h>

namespace iloj::gpu
{
namespace image
{
auto Quad() -> Drawable &;

template<typename T = Texture2D, typename... ENTRIES>
void execute(TargetList<T> targets,
             const ViewPort &viewPort,
             const Clear::Context &clearContext,
             const Blending::Context &blendingContext,
             iloj::gpu::Program &program,
             ENTRIES... entries)
{
    execute(targets,
            viewPort,
            clearContext,
            blendingContext,
            Depth::Context::None(),
            Culling::Context::None(),
            Quad(),
            program,
            std::forward<ENTRIES>(entries)...);
}

auto make_pyramid(unsigned w,
                  unsigned h,
                  int internal_format,
                  int interpolation,
                  int wrap,
                  float scale,
                  unsigned nbLevel = std::numeric_limits<unsigned>::max()) -> std::vector<Texture2D>;

void iterate(unsigned iter,
             Texture2D &inputBuffer,
             Texture2D &outputBuffer,
             const std::function<void(Texture2D &)> &initPlan,
             const std::function<void(unsigned, const Texture2D &, Texture2D &)> &iterPlan);

class SeparableFilter
{
private:
    std::vector<float> m_vFilter;
    std::vector<float> m_hFilter;
    std::vector<float> m_dFilter;

public:
    SeparableFilter(std::vector<float> vFilter, std::vector<float> hFilter, std::vector<float> dFilter = {})
        : m_vFilter(std::move(vFilter)), m_hFilter(std::move(hFilter)), m_dFilter(std::move(dFilter))
    {
    }
    [[nodiscard]] auto getVerticalFilter() const -> const std::vector<float> & { return m_vFilter; }
    [[nodiscard]] auto getHorizontalFilter() const -> const std::vector<float> & { return m_hFilter; }
    [[nodiscard]] auto getDepthFilter() const -> const std::vector<float> & { return m_dFilter; }
};

void filter(const Texture2D &inputBuffer, Texture2D &outputBuffer, const SeparableFilter &f);
void filter(const Texture2DArray &inputBuffer, Texture2DArray &outputBuffer, const SeparableFilter &f);

namespace Program
{
template<typename T = float>
auto channel() -> iloj::gpu::Program &;
template<typename T = float>
auto copy() -> iloj::gpu::Program &;
template<typename T = float>
auto flip() -> iloj::gpu::Program &;
auto display() -> iloj::gpu::Program &;
auto upscale() -> iloj::gpu::Program &;
auto hfilter() -> iloj::gpu::Program &;
auto vfilter() -> iloj::gpu::Program &;
auto dfilter() -> iloj::gpu::Program &;
template<unsigned N = 3>
auto median() -> iloj::gpu::Program &;
auto scale() -> iloj::gpu::Program &;

auto rgb2luma() -> iloj::gpu::Program &;
auto luma2rgb() -> iloj::gpu::Program &;
auto yuv2rgb() -> iloj::gpu::Program &;
auto nv2rgb() -> iloj::gpu::Program &;
auto yuv2luma() -> iloj::gpu::Program &;
template<typename OUTPUT = float>
auto nv2luma() -> iloj::gpu::Program &;
auto convert() -> iloj::gpu::Program &;
auto rgb2yuv() -> iloj::gpu::Program &;
auto mediacodec_yuv2rgb() -> iloj::gpu::Program &;
auto rgb2lab() -> iloj::gpu::Program &;
auto oes2rgb() -> iloj::gpu::Program &;
auto oes2yuv() -> iloj::gpu::Program &;

static const std::string VertexShader = R"DEF_SHADER(
in vec3 Vertex;
out vec2 texCoord;

void main()
{
	gl_Position = vec4(Vertex.xy, 0., 1.);
	texCoord = 0.5 * (Vertex.xy + 1.);
}
)DEF_SHADER";
} // namespace Program

auto getGLFormat(const iloj::media::Descriptor::Video &d, unsigned plane) -> unsigned;

class Importer
{
public:
    enum class Mode
    {
        Luma,
        Color,
        Integral
    };

private:
    std::array<Texture2D, 4> m_inputs;
    Texture2D m_intermediate;

public:
    void load(const iloj::media::Descriptor::Video &d,
              Texture2D &out,
              Mode mode,
              unsigned flip,
              unsigned interpolationMode,
              unsigned wrapMode,
              const iloj::media::ColorProfile &colorProfile = iloj::media::ColorProfile::BT709());
    [[nodiscard]] auto getPlane(unsigned id) const -> const Texture2D & { return m_inputs[id]; }
    static auto load(const iloj::media::Descriptor::Video &d,
                     Mode mode,
                     unsigned flip,
                     unsigned interpolationMode,
                     unsigned wrapMode,
                     const iloj::media::ColorProfile &colorProfile = iloj::media::ColorProfile::BT709()) -> Texture2D
    {
        static Importer importer;

        Texture2D out;
        importer.load(d, out, mode, flip, interpolationMode, wrapMode, colorProfile);
        return out;
    }

private:
    void convertToLuma(const media::Descriptor::Video &d, Texture2D &out, const media::ColorProfile &colorProfile);
    void convertToColor(const media::Descriptor::Video &d, Texture2D &out, const media::ColorProfile &colorProfile);
    void convertToIntegral(const media::Descriptor::Video &d, Texture2D &out);
};

void convert(const Texture2D &source,
             Texture2D &target,
             const iloj::media::ColorProfile &sourceProfile,
             const iloj::media::ColorProfile &targetProfile);

auto rgb2yuv(const Texture2D &rgb, unsigned pixelFormatId, const iloj::media::YCC &ycc, int rangeMode)
    -> iloj::media::Descriptor::Video;

template<typename PIXELFORMAT>
auto rgb2yuv(const Texture2D &rgb, const iloj::media::YCC &ycc, int rangeMode) -> iloj::media::Descriptor::Video
{
    return rgb2yuv(rgb, PIXELFORMAT::Id, ycc, rangeMode);
}

void sat(const Texture2D &I, Texture2D &SAT);
inline auto sat(const Texture2D &I) -> Texture2D
{
    Texture2D SAT;
    sat(I, SAT);
    return SAT;
}

class CCL
{
public:
    class Parameter
    {
    private:
        unsigned m_outerIter = 4;
        unsigned m_innerIter = 4;
        unsigned m_searchDepth = 8;

    public:
        Parameter(unsigned outerIter = 4, unsigned innerIter = 4, unsigned searchDepth = 8)
            : m_outerIter{outerIter}, m_innerIter{innerIter}, m_searchDepth{searchDepth}
        {
        }
        [[nodiscard]] auto getOuterIter() const -> unsigned { return m_outerIter; }
        [[nodiscard]] auto getInnerIter() const -> unsigned { return m_innerIter; }
        [[nodiscard]] auto getSearchDepth() const -> unsigned { return m_searchDepth; }
    };

private:
    iloj::gpu::Program m_connectionProgram;
    Drawable m_connectionDrawable;
    std::array<Texture2D, 2> m_buffer;

public:
    explicit CCL(const std::string &componentShader);
    void label(const Texture2D &input, const Parameter &parameter = {});
    auto getDrawable() -> Drawable & { return m_connectionDrawable; }
    [[nodiscard]] auto getOutput() const -> const Texture2D & { return m_buffer[1]; }
};

namespace shrink
{
void nearest(const Texture2D &input, Texture2D &output, const Vec2u &size);
inline auto nearest(const Texture2D &input, const Vec2u &size)
{
    Texture2D output;
    nearest(input, output, size);
    return output;
}

void lanczos(const Texture2D &input, Texture2D &output, const Vec2u &size);
inline auto lanczos(const Texture2D &input, const Vec2u &size)
{
    Texture2D output;
    lanczos(input, output, size);
    return output;
}
} // namespace shrink

class Interactor: public iloj::gpu::Interactor
{
protected:
    Interactor::MouseButton m_moveButton = Interactor::MouseButton::Left;
    bool m_moveButtonPressed = false;
    iloj::math::Vec2f m_moveScaling = {1.F, 1.F};
    float m_scrollScaling = 1.F;
    float m_imageAspectRatio = 1.F;
    iloj::math::Vec2i m_screenPos = {-1, -1};
    iloj::math::Vec2f m_cursorPos = {0.F, 0.F};
    iloj::math::Vec2f m_roiCenter = {0.5F, 0.5F};
    float m_roiZoom = 1.F;

public:
    void setMoveButton(Interactor::MouseButton button) { m_moveButton = button; }
    void setMoveScaling(Vec2f moveScaling) { m_moveScaling = moveScaling; }
    void setScrollScaling(float scrollScaling) { m_scrollScaling = scrollScaling; }
    void setImageAspectRatio(float imageAspectRatio) { m_imageAspectRatio = imageAspectRatio; }
    auto getCenterPosition() const -> const iloj::math::Vec2f & { return m_roiCenter; }
    auto getZoomFactor() const -> float { return m_roiZoom; }
    auto getCursorPosition() const -> const iloj::math::Vec2f & { return m_cursorPos; }

public:
    void onMouseButtonPress(Interactor::MouseButton button, int x, int y) override;
    void onMouseButtonRelease(Interactor::MouseButton button, int x, int y) override;
    void onMouseMove(int x, int y) override;
    void onScroll(int dx, int dy) override;
};

} // namespace image
} // namespace iloj::gpu
