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
#include "functions.h"
#include "texture.h"

namespace iloj::gpu
{
namespace Uniform
{
template<typename T>
using Entry = std::pair<std::string, const T &>;

class Base
{
public:
    Base() = default;
    Base(const Base &) = delete;
    Base(Base &&) = default;
    virtual ~Base() = default;
    auto operator=(const Base &) -> Base & = delete;
    auto operator=(Base &&) -> Base & = default;
    [[nodiscard]] virtual auto isValid() const -> bool { return false; }
    [[nodiscard]] virtual auto isBuffer() const -> bool { return false; }
    static auto Default() -> Base &
    {
        static Base b;
        return b;
    }
};

class Regular: public Base
{
private:
    int m_id = -1;

public:
    Regular(const std::string &name, unsigned program_id);
    ~Regular() override = default;
    Regular(const Regular &) = delete;
    Regular(Regular &&) = default;
    auto operator=(const Regular &) -> Regular & = delete;
    auto operator=(Regular &&) -> Regular & = default;
    [[nodiscard]] auto isValid() const -> bool override { return (-1 != m_id); }
    [[nodiscard]] auto isBuffer() const -> bool override { return false; }
    template<typename T>
    void setValue(const T &v)
    {
        if (isValid())
        {
            applyValue(v);
        }
    }
    static auto Default() -> Regular &
    {
        static Regular b;
        return b;
    }

private:
    template<typename T>
    void applyValue(const T &v);

private:
    Regular() = default;
};

class Buffer: public Base, public gpu::Buffer::Model
{
public:
    class Layout
    {
        friend class Buffer;

    public:
        enum class Array
        {
            On,
            Off
        };
        enum class Struct
        {
            On,
            Off
        };

    private:
        std::vector<std::uint8_t> m_buffer;
        std::size_t m_offset = 0;

    public:
        void reset() { m_offset = 0; }
        [[nodiscard]] auto tellg() const -> std::size_t { return m_offset; }
        template<typename T>
        void align()
        {
            std::size_t misalignment = m_offset % sizeof(T);
            if (misalignment != 0U)
            {
                m_offset += sizeof(T) - misalignment;
            }
        }
        template<typename T>
        void write(const T &v)
        {
            const auto *ptr = reinterpret_cast<const std::uint8_t *>(&v);
            std::copy(ptr, ptr + sizeof(T), m_buffer.begin() + m_offset);
            m_offset += sizeof(T);
        }
        template<typename InputIt>
        void write(InputIt first, InputIt last)
        {
            for (auto iter = first; iter != last; iter++)
            {
                write(*iter);
            }
        }
        [[nodiscard]] auto data() const -> const std::uint8_t * { return m_buffer.data(); }
        template<typename T>
        auto operator<<(const T &value) -> Layout &;

    private:
        void setBlockSize(std::size_t sz) { m_buffer.resize(sz); }
    };

private:
    unsigned m_blockIndex = GL_INVALID_INDEX, m_blockBinding{};
    int m_blockSize = 0;
    Layout m_layout;

public:
    Buffer(const std::string &name, unsigned binding, unsigned program_id, unsigned mode = GL_DYNAMIC_DRAW);
    ~Buffer() override = default;
    Buffer(const Buffer &) = delete;
    Buffer(Buffer &&) = default;
    auto operator=(const Buffer &) -> Buffer & = delete;
    auto operator=(Buffer &&) -> Buffer & = default;
    [[nodiscard]] auto isValid() const -> bool override { return (m_blockIndex != GL_INVALID_INDEX); }
    [[nodiscard]] auto isBuffer() const -> bool override { return true; }
    [[nodiscard]] auto getBlockSize() const -> int { return m_blockSize; }
    template<typename B>
    void setValue(const B &buffer)
    {
        if (isValid())
        {
            // Updating layout
            buffer.toUniformBufferLayout(m_layout);

            // Sending to GPU
            bind();
            update(0, m_blockSize, (void *) m_layout.data());
            unbind();

            // Clearing layout
            m_layout.reset();

            // Bind to uniform
            glBindBufferBase(GL_UNIFORM_BUFFER, m_blockBinding, getId());
        }
    }
    static auto Default() -> Buffer &
    {
        static Buffer b;
        return b;
    }

private:
    Buffer() = default;
    ;
};

template<typename T>
class Type
{
private:
    using Yes = std::uint8_t;
    using No = std::uint16_t;

    template<typename C>
    static auto Test(void *)
        -> decltype(std::declval<C const>().toUniformBufferLayout(std::declval<gpu::Uniform::Buffer::Layout &>()),
                    Yes{});
    template<typename>
    static auto Test(...) -> No &;

public:
    static bool const isRegular = (sizeof(Test<T>(nullptr)) == sizeof(No)); // NOLINT
    static bool const isBuffer = (sizeof(Test<T>(nullptr)) == sizeof(Yes)); // NOLINT
};
} // namespace Uniform
} // namespace iloj::gpu
