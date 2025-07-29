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

#include "lookat.h"
#include "uniform.h"
#include <iloj/math/pose.h>

namespace iloj::gpu
{
class Camera
{
public:
    struct ProjectionId
    {
        enum
        {
            None = 0,
            Perspective,
            Equirectangular,
            CubeMap
        };
    };

    class List: public std::vector<Camera>
    {
    public:
        using std::vector<Camera>::vector;
        using std::vector<Camera>::operator=;
        auto operator==(const List &other) const -> bool
        {
            return ((size() == other.size()) && std::equal(begin(), end(), other.begin()));
        }
        auto operator!=(const List &other) const -> bool { return !(*this == other); }
        void toUniformBufferLayout(iloj::gpu::Uniform::Buffer::Layout &layout) const
        {
            layout << static_cast<unsigned>(size());
            for (const auto &c : *this)
            {
                c.toUniformBufferLayout(layout);
            }
        }
    };

private:
    int m_projectionId{};
    Vec2u m_viewportSize{};
    Vec4f m_intrinsicParameter{};
    Vec2f m_clippingRange{};
    LookAt m_lookAt;

public:
    Camera(): m_lookAt(getLocalFrame()) {}
    ~Camera() = default;
    Camera(const Camera &) = default;
    Camera(Camera &&) = default;
    auto operator=(const Camera &) -> Camera & = default;
    auto operator=(Camera &&) -> Camera & = default;
    static auto Perspective(const Vec2u &viewportSize, float verticalFoV, const Vec2f &clippingRange) -> Camera;
    static auto Perspective(const Vec2u &viewportSize,
                            const Vec2f &focalLength,
                            const Vec2f &principalPoint,
                            const Vec2f &clippingRange) -> Camera;
    static auto Equirectangular(const Vec2u &viewportSize,
                                const Vec2f &longRange,
                                const Vec2f &latRange,
                                const Vec2f &clippingRange) -> Camera;
    static auto CubeMap(const Vec2u &viewportSize, const Vec2f &clippingRange) -> Camera;
    void setProjectionId(int projectionId) { m_projectionId = projectionId; }
    [[nodiscard]] auto getProjectionId() const -> int { return m_projectionId; }
    void setViewportSize(const Vec2u &viewportSize) { m_viewportSize = viewportSize; }
    [[nodiscard]] auto getViewportSize() const -> const Vec2u & { return m_viewportSize; }
    void setIntrinsicParameter(const Vec4f &intrinsicParameter) { m_intrinsicParameter = intrinsicParameter; }
    [[nodiscard]] auto getIntrinsicParameter() const -> const Vec4f & { return m_intrinsicParameter; }
    void setClippingRange(const Vec2f &clippingRange) { m_clippingRange = clippingRange; }
    [[nodiscard]] auto getClippingRange() const -> const Vec2f & { return m_clippingRange; }
    void setVerticalFoV(float verticalFoV);
    [[nodiscard]] auto getVerticalFoV() const -> float;
    void setHorizontalFoV(float horizontalFoV);
    [[nodiscard]] auto getHorizontalFoV() const -> float;
    [[nodiscard]] auto getLookAt() const -> const LookAt & { return m_lookAt; }
    auto getLookAt() -> LookAt & { return m_lookAt; }
    auto operator==(const Camera &other) const -> bool
    {
        return ((m_projectionId == other.m_projectionId) && (m_viewportSize == other.m_viewportSize) &&
                (m_intrinsicParameter == other.m_intrinsicParameter) && (m_clippingRange == other.m_clippingRange) &&
                ((m_lookAt == other.m_lookAt)));
    }
    auto operator!=(const Camera &other) const -> bool { return !(*this == other); }
    void toUniformBufferLayout(iloj::gpu::Uniform::Buffer::Layout &layout) const
    {
        auto mGlobalToLocal = m_lookAt.getGlobalToLocal().toMatrix();
        auto mLocalToGlobal = m_lookAt.getLocalToGlobal().toMatrix();

        layout << Uniform::Buffer::Layout::Struct::On << m_projectionId << m_viewportSize << m_intrinsicParameter
               << m_clippingRange << mGlobalToLocal << mLocalToGlobal << Uniform::Buffer::Layout::Struct::Off;
    }
    static auto getLocalFrame() -> const iloj::math::Frame &;
    static auto getShaderCode() -> const std::string &;
    [[nodiscard]] auto doProjection(const Vec3f &Q) const -> Vec3f;
    [[nodiscard]] auto doUnprojection(const Vec3f &UVz) const -> Vec3f;
};
} // namespace iloj::gpu
