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

#include "memory.h"
#include "texture.h"
#include <limits>
#include <memory>

namespace iloj::gpu
{
class FrameBuffer: public Object::Base
{
public:
    class Attachment
    {
    public:
        enum class TypeId
        {
            RENDERBUFFER = 0,
            TEXTURE2D,
            TEXTURE2DMS
        };
        class Model
        {
        private:
            TypeId m_type;
            FrameBuffer &m_parent;
            unsigned m_role;

        public:
            Model(TypeId type, FrameBuffer &parent, unsigned role);
            [[nodiscard]] auto getType() const -> TypeId { return m_type; }
            [[nodiscard]] auto getParent() const -> FrameBuffer & { return m_parent; }
            [[nodiscard]] auto getRole() const -> unsigned { return m_role; }
        };
        class RenderBuffer: public Object::Base, public Model
        {
        private:
            unsigned m_id{std::numeric_limits<unsigned>::max()};

        public:
            RenderBuffer(FrameBuffer &parent, unsigned role);
            ~RenderBuffer() override;
            RenderBuffer(const RenderBuffer &) = delete;
            RenderBuffer(RenderBuffer && /*other*/) noexcept;
            auto operator=(const RenderBuffer &) -> RenderBuffer & = delete;
            auto operator=(RenderBuffer && /*other*/) noexcept -> RenderBuffer &;
            void set(unsigned width, unsigned height, unsigned format, unsigned samples) const;

        protected:
            void destroy() override { cleanup(); }

        private:
            void cleanup();
        };
        class Texture2D: public Model
        {
        public:
            Texture2D(FrameBuffer &parent, unsigned role);
            void set(const gpu::Texture2D &texture);
            void clear();
        };
        class Texture2DMS: public Model
        {
        public:
            Texture2DMS(FrameBuffer &parent, unsigned role);
            void set(const gpu::Texture2DMS &texture);
            void clear();
        };
    };

private:
    unsigned m_id{std::numeric_limits<unsigned>::max()};
    std::vector<std::unique_ptr<Attachment::Model>> m_colorAttachment;
    std::unique_ptr<Attachment::Model> m_depthAttachment;

public:
    FrameBuffer();
    FrameBuffer(const FrameBuffer &) = delete;
    FrameBuffer(FrameBuffer && /*fb*/) noexcept;
    ~FrameBuffer() override;
    auto operator=(const FrameBuffer &) -> FrameBuffer & = delete;
    auto operator=(FrameBuffer && /*fb*/) noexcept -> FrameBuffer &;
    [[nodiscard]] auto id() const -> unsigned { return m_id; }
    void bind() const;
    static void unbind();
    [[nodiscard]] auto getNumberOfColorAttachment() const -> unsigned { return m_colorAttachment.size(); }
    template<typename A>
    void addColorAttachment()
    {
        m_colorAttachment.push_back(std::make_unique<A>(*this, GL_COLOR_ATTACHMENT0 + m_colorAttachment.size()));
    }
    template<typename A = Attachment::Model>
    [[nodiscard]] [[nodiscard]] auto getColorAttachment(unsigned i = 0) const -> const A &
    {
        return static_cast<const A &>(*m_colorAttachment[i]);
    }
    template<typename A = Attachment::Model>
    auto getColorAttachment(unsigned i = 0) -> A &
    {
        return static_cast<A &>(*m_colorAttachment[i]);
    }
    template<typename A = Attachment::Model>
    void setDepthAttachment()
    {
        m_depthAttachment = std::make_unique<A>(*this, GL_DEPTH_ATTACHMENT);
    }
    template<typename A = Attachment::Model>
    [[nodiscard]] auto getDepthAttachment() const -> const A &
    {
        return static_cast<const A &>(*m_depthAttachment);
    }
    template<typename A = Attachment::Model>
    auto getDepthAttachment() -> A &
    {
        return static_cast<A &>(*m_depthAttachment);
    }
    [[nodiscard]] auto getColorAttachments() const -> const std::vector<std::unique_ptr<Attachment::Model>> &
    {
        return m_colorAttachment;
    }
    static void setReadBuffer(unsigned readBuffer);
    static void disableReadBuffer();
    static void setDrawBuffers(std::vector<unsigned> drawBuffers);
    static void disableDrawBuffers();
    void
    readPixels(unsigned buffer, int x, int y, int width, int height, unsigned format, unsigned type, void *data) const;
    static void blit(FrameBuffer &fbo_source,
                     FrameBuffer &fbo_target,
                     unsigned readBuffer,
                     unsigned drawBuffer,
                     int srcX0,
                     int srcY0,
                     int srcX1,
                     int srcY1,
                     int dstX0,
                     int dstY0,
                     int dstX1,
                     int dstY1,
                     unsigned mask,
                     unsigned filter);

protected:
    void destroy() override { cleanup(); }

private:
    void cleanup();
};
} // namespace iloj::gpu
