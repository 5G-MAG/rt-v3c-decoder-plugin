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

#include "framebuffer.h"

namespace iloj::gpu
{
auto getAttachmentType(const Texture2D &) -> FrameBuffer::Attachment::Texture2D;
auto getAttachmentType(const Texture2DMS &) -> FrameBuffer::Attachment::Texture2DMS;

auto getDepthBufferType(const Texture2D &) -> Texture2D;
auto getDepthBufferType(const Texture2DMS &) -> Texture2DMS;

template<typename T>
class RenderToTexture
{
public:
    using DepthBufferType = decltype(getDepthBufferType(T()));

private:
    FrameBuffer m_frameBuffer;
    DepthBufferType m_depthBuffer;

public:
    RenderToTexture();
    ~RenderToTexture() = default;
    RenderToTexture(const RenderToTexture &) = delete;
    RenderToTexture(RenderToTexture &&) noexcept = default;
    auto operator=(const RenderToTexture &) -> RenderToTexture & = delete;
    auto operator=(RenderToTexture &&) noexcept -> RenderToTexture & = default;
    auto getFrameBuffer() -> FrameBuffer & { return m_frameBuffer; }
    void attach(const T &target, unsigned attachmentId = 0);
    void detach(unsigned attachmentId = 0);
    void execute(T &target, std::function<void()> plan) { execute(target, m_depthBuffer, plan); }
    void execute(TargetList<T> targets, std::function<void()> plan) { execute(targets, m_depthBuffer, plan); }
    void execute(T &target, DepthBufferType &depth, const std::function<void()> &plan);
    void execute(TargetList<T> targets, DepthBufferType &depth, const std::function<void()> &plan);
    static void reshapeDepthBuffer(const T &colorBuffer, DepthBufferType &depthBuffer);
};

class RenderToTextureManager
{
private:
    std::unique_ptr<RenderToTexture<Texture2D>> m_rttTexture2D;
    std::unique_ptr<RenderToTexture<Texture2DMS>> m_rttTexture2DMS;

public:
    template<typename T>
    auto getRenderToTexture() -> RenderToTexture<T> &;
};

} // namespace iloj::gpu

#include "rtt.hpp"
