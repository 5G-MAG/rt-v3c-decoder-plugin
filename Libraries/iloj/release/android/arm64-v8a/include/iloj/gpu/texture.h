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

#include "buffer.h"
#include "memory.h"
#include "viewport.h"

namespace iloj::gpu
{
namespace Texture
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Model: public Object::Base
{
protected:
    unsigned m_class = 0;
    unsigned m_id = std::numeric_limits<unsigned>::max();
    unsigned m_size = 0;
    bool m_managed = false;
    unsigned m_internalFormat = 0;
    unsigned m_interpolationMode = 0;
    unsigned m_wrapMode = 0;

public:
    explicit Model(unsigned myclass, int id = -1);
    Model(const Model &) = delete;
    Model(Model && /*m*/) noexcept;
    ~Model() override;
    auto operator=(const Model &) -> Model & = delete;
    auto operator=(Model && /*m*/) noexcept -> Model &;
    void bind() const;
    void unbind() const;
    [[nodiscard]] auto getClass() const -> unsigned { return m_class; }
    [[nodiscard]] auto getId() const -> unsigned { return m_id; }
    [[nodiscard]] auto getSize() const -> unsigned { return m_size; }
    [[nodiscard]] auto getInternalFormat() const -> unsigned { return m_internalFormat; }
    [[nodiscard]] auto getInterpolationMode() const -> unsigned { return m_interpolationMode; }
    void setInterpolationMode(unsigned interpolationMode);
    [[nodiscard]] auto getWrapMode() const -> unsigned { return m_wrapMode; }
    void setWrapMode(unsigned wrapMode);
    void generateMipMap(unsigned max_level = 1000) const;
    [[nodiscard]] auto getNumberOfChannels() const -> unsigned;
    [[nodiscard]] auto getBytePerChannel() const -> unsigned;
    [[nodiscard]] auto isNormalizedIntegerType() const -> bool;
    [[nodiscard]] auto isIntegerType() const -> bool;
    [[nodiscard]] auto isUnsignedIntegerType() const -> bool;
    [[nodiscard]] auto isFloatingType() const -> bool;
    [[nodiscard]] auto getShaderOutputTypeStringFromInternalFormat() const -> std::string;
    static void getFormatAndTypeFromInternalFormat(unsigned &internal_format, unsigned &format, unsigned &type);

protected:
    void setInterpolationModeInternal(unsigned interpolationMode);
    void setWrapModeInternal(unsigned wrapMode);
    void destroy() override { cleanup(); }

private:
    void cleanup();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SlotManager
{
private:
    unsigned m_firstAvailable = 0;

public:
    auto acquire(unsigned n = 1) -> unsigned
    {
        unsigned out = m_firstAvailable;
        m_firstAvailable += n;
        return out;
    }
    void clear() { m_firstAvailable = 0; }
};
} // namespace Texture

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Image
{
private:
    const Texture::Model &m_parent;
    int m_access{GL_NONE};
    int m_level{};

public:
    Image(const Texture::Model &parent, int access, int level = 0): m_parent{parent}, m_access{access}, m_level{level}
    {
    }
    Image(const Image &) = default;
    Image(Image &&) noexcept = default;
    ~Image() = default;
    auto operator=(const Image &) -> Image & = delete;
    auto operator=(Image &&) noexcept -> Image & = delete;
    auto getParent() const -> const Texture::Model & { return m_parent; }
    auto getAccess() const -> int { return m_access; }
    auto getLevel() const -> int { return m_level; }
    auto isLayered() const -> bool { return (m_parent.getClass() == GL_TEXTURE_2D_ARRAY); }
};

class ImageES: public Image
{
public:
    ImageES(const Texture::Model &parent, int access, int level = 0): Image(parent, access, level) {}
    ImageES(const ImageES &) = default;
    ImageES(ImageES &&) noexcept = default;
    ~ImageES() = default;
    auto operator=(const ImageES &) -> ImageES & = delete;
    auto operator=(ImageES &&) noexcept -> ImageES & = delete;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TextureBuffer: public Texture::Model
{
private:
    Buffer::Texture m_buffer;

public:
    TextureBuffer(): Model(GL_TEXTURE_BUFFER) {}
    ~TextureBuffer() override = default;
    TextureBuffer(const TextureBuffer &) = delete;
    TextureBuffer(TextureBuffer &&) = default;
    auto operator=(const TextureBuffer &) -> TextureBuffer & = delete;
    auto operator=(TextureBuffer &&) -> TextureBuffer & = default;
    auto getBuffer() -> Buffer::Texture & { return m_buffer; }
    template<typename T>
    void reshape(unsigned n, unsigned mode = GL_STATIC_DRAW)
    {
        m_internalFormat = getInternalFormat<T>();
        m_buffer.reshape<T>(n, mode);

        bind();
        glTexBuffer(GL_TEXTURE_BUFFER, m_internalFormat, m_buffer.getId());
        unbind();

        m_size = n;
    }
    template<typename T>
    void update(const T *data, unsigned nb_element, unsigned beg_element = 0, unsigned mode = GL_STATIC_DRAW)
    {
        unsigned n = beg_element + nb_element;

        m_internalFormat = getInternalFormat<T>();

        if (m_size < n)
        {
            if (beg_element == 0)
            {
                m_buffer.reshapeFromData(data, nb_element, mode);

                bind();
                glTexBuffer(GL_TEXTURE_BUFFER, m_internalFormat, m_buffer.getId());
                unbind();

                m_size = n;
            }
            else
            {
                m_buffer.reshape<T>(n, mode);

                bind();
                glTexBuffer(GL_TEXTURE_BUFFER, m_internalFormat, m_buffer.getId());
                unbind();

                m_size = n;

                m_buffer.update(data, nb_element, beg_element);
            }
        }
        else
        {
            m_buffer.update(data, nb_element, beg_element);
        }
    }
    template<typename CONTAINER>
    void update(const CONTAINER &data, unsigned beg_element = 0, unsigned mode = GL_STATIC_DRAW)
    {
        update(data.data(), data.size(), beg_element, mode);
    }
    template<typename T>
    void getData(void *buffer, unsigned nb_elements, unsigned beg_element = 0) const
    {
        m_buffer.getData<T>(buffer, nb_elements, beg_element);
    }
    template<typename T>
    static auto create(unsigned n, unsigned mode = GL_STATIC_DRAW)
    {
        TextureBuffer output;
        output.reshape<T>(n, mode);
        return output;
    }
    template<typename T>
    static auto create(unsigned n, const T *data, unsigned beg_element = 0, unsigned mode = GL_STATIC_DRAW)
    {
        TextureBuffer output;
        output.update<T>(data, n, beg_element, mode);
        return output;
    }
    template<typename CONTAINER>
    static auto create(const CONTAINER &data, unsigned beg_element = 0, unsigned mode = GL_STATIC_DRAW)
    {
        TextureBuffer output;
        output.update<CONTAINER>(data, beg_element, mode);
        return output;
    }
    static auto getMaxSize() -> int
    {
        int v = 0;
        glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &v);
        return v;
    }

private:
    template<typename T>
    auto getInternalFormat() -> int;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Texture1D: public Texture::Model
{
public:
    explicit Texture1D(int id = -1);
    Texture1D(unsigned sz, int internal_format, int interpolation, int wrap): Model(GL_TEXTURE_1D)
    {
        reshape(sz, internal_format, interpolation, wrap);
    }
    ~Texture1D() override = default;
    Texture1D(const Texture1D &) = delete;
    Texture1D(Texture1D &&) = default;
    auto operator=(const Texture1D &) -> Texture1D & = delete;
    auto operator=(Texture1D &&) -> Texture1D & = default;
    void reshape(unsigned sz, unsigned internal_format, unsigned interpolation, unsigned wrap);
    void setContent(unsigned sz, unsigned internal_format, const void *data, unsigned interpolation, unsigned wrap);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Texture2D: public Texture::Model
{
    friend class Texture2DArray;

private:
    unsigned m_width = 0, m_height = 0;

public:
    explicit Texture2D(int id = -1);
    Texture2D(int id, unsigned w, unsigned h, unsigned internal_format, unsigned interpolation, unsigned wrap)
        : Model(GL_TEXTURE_2D, id)
    {
        if (id < 0)
        {
            reshape(w, h, internal_format, interpolation, wrap);
        }
        else
        {
            m_width = w;
            m_height = h;
            m_size = w * h;
            m_internalFormat = internal_format;
        }
    }
    Texture2D(unsigned w, unsigned h, unsigned internal_format, unsigned interpolation, unsigned wrap)
        : Model(GL_TEXTURE_2D)
    {
        reshape(w, h, internal_format, interpolation, wrap);
    }
    Texture2D(unsigned w,
              unsigned h,
              int internal_format,
              const void *data,
              unsigned interpolation,
              unsigned wrap,
              unsigned alignment = std::numeric_limits<unsigned>::max(),
              bool swap = false)
        : Model(GL_TEXTURE_2D)
    {
        setContent(w, h, internal_format, data, interpolation, wrap, alignment, swap);
    }
    ~Texture2D() override = default;
    Texture2D(const Texture2D &) = delete;
    Texture2D(Texture2D &&) noexcept = default;
    auto operator=(const Texture2D &) -> Texture2D & = delete;
    auto operator=(Texture2D &&) noexcept -> Texture2D & = default;
    void reshape(unsigned w, unsigned h, unsigned internal_format, unsigned interpolation, unsigned wrap);
    void setContent(unsigned w,
                    unsigned h,
                    unsigned internal_format,
                    const void *data,
                    unsigned interpolation,
                    unsigned wrap,
                    unsigned alignment = std::numeric_limits<unsigned>::max(),
                    bool swap = false);
    [[nodiscard]] auto width() const -> unsigned { return m_width; }
    [[nodiscard]] auto height() const -> unsigned { return m_height; }
    [[nodiscard]] auto getViewPort() const -> ViewPort { return {m_width, m_height}; }
    void getData(void *buffer, unsigned format, unsigned type) const;
    void savePGM(const std::string &path,
                 unsigned flip = FlipMode::Vertical,
                 std::vector<float> scaleRange = {},
                 unsigned component = 0) const;
    void savePPM(const std::string &path, unsigned flip = FlipMode::Vertical) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Texture2DMS: public Texture::Model
{
private:
    unsigned m_width = 0, m_height = 0;
    unsigned m_samples = 0;
    bool m_fixed = false;

public:
    explicit Texture2DMS(int id = -1);
    Texture2DMS(unsigned w, unsigned h, int internal_format, unsigned nb_samples, bool fixed)
        : Model(GL_TEXTURE_2D_MULTISAMPLE)
    {
        reshape(w, h, internal_format, nb_samples, fixed);
    }
    ~Texture2DMS() override = default;
    Texture2DMS(const Texture2DMS &) = delete;
    Texture2DMS(Texture2DMS &&) noexcept = default;
    auto operator=(const Texture2DMS &) -> Texture2DMS & = delete;
    auto operator=(Texture2DMS &&) noexcept -> Texture2DMS & = default;
    void reshape(unsigned w, unsigned h, unsigned internal_format, unsigned nb_samples, bool fixed);
    [[nodiscard]] auto width() const -> unsigned { return m_width; }
    [[nodiscard]] auto height() const -> unsigned { return m_height; }
    [[nodiscard]] auto getViewPort() const -> ViewPort { return {m_width, m_height}; }
    [[nodiscard]] auto samples() const -> unsigned { return m_samples; }
    [[nodiscard]] auto isFixed() const -> bool { return m_fixed; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Texture2DArray: public Texture::Model
{
private:
    unsigned m_width = 0;
    unsigned m_height = 0;
    unsigned m_depth = 0;
    std::vector<Texture2D> m_views;

public:
    explicit Texture2DArray(int id = -1);
    Texture2DArray(unsigned w, unsigned h, unsigned d, unsigned internal_format, unsigned interpolation, unsigned wrap)
        : Model(GL_TEXTURE_2D_ARRAY)
    {
        reshape(w, h, d, internal_format, interpolation, wrap);
    }
    ~Texture2DArray() override = default;
    Texture2DArray(const Texture2DArray &) = delete;
    Texture2DArray(Texture2DArray &&) noexcept = default;
    auto operator=(const Texture2DArray &) -> Texture2DArray & = delete;
    auto operator=(Texture2DArray &&) noexcept -> Texture2DArray & = default;
    [[nodiscard]] auto width() const -> unsigned { return m_width; }
    [[nodiscard]] auto height() const -> unsigned { return m_height; }
    [[nodiscard]] auto depth() const -> unsigned { return m_depth; }
    auto getView(unsigned layerId) -> Texture2D & { return m_views[layerId]; }
    [[nodiscard]] auto getView(unsigned layerId) const -> const Texture2D & { return m_views[layerId]; }
    void getData(void *buffer, unsigned format, unsigned type) const;

private:
    void reshape(unsigned w, unsigned h, unsigned d, unsigned internal_format, unsigned interpolation, unsigned wrap);
    void updateView(bool reallocation);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TextureExternalOES: public Texture::Model
{
private:
    class JavaContext;
    std::unique_ptr<JavaContext> m_javaContext;

public:
    TextureExternalOES();
    ~TextureExternalOES() override;
    TextureExternalOES(const TextureExternalOES &) = delete;
    TextureExternalOES(TextureExternalOES &&) noexcept = delete;
    auto operator=(const TextureExternalOES &) -> TextureExternalOES & = delete;
    auto operator=(TextureExternalOES &&) noexcept -> TextureExternalOES & = delete;
    void waitForFrame();
    void updateTexImage();
    void releaseTexImage();
    [[nodiscard]] auto getTransformMatrix() const -> Mat4x4f;
    auto getSurface() -> void *;
};

} // namespace iloj::gpu
