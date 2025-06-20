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

#include "functions.h"
#include "memory.h"
#include "types.h"
#include <type_traits>

namespace iloj::gpu
{
namespace Buffer
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Model: public Object::Base
{
private:
    unsigned m_target{};
    unsigned m_id{std::numeric_limits<unsigned>::max()};

public:
    explicit Model(unsigned target);
    Model(const Model &) = delete;
    Model(Model &&other) noexcept: m_target(other.m_target), m_id(other.m_id)
    {
        other.m_target = 0;
        other.m_id = std::numeric_limits<unsigned>::max();
    }
    ~Model() override;
    auto operator=(const Model &) -> Model & = delete;
    auto operator=(Model &&other) noexcept -> Model &;
    [[nodiscard]] auto getId() const -> unsigned { return m_id; }
    void bind() const;
    void unbind() const;
    void update(unsigned offset, unsigned size, const void *data) const;
    template<typename T>
    void update(const T *data, unsigned nb_element, unsigned beg_element = 0)
    {
        update(beg_element * sizeof(T), nb_element * sizeof(T), data);
    }
    template<typename CONTAINER>
    void update(const CONTAINER &data, unsigned beg_element = 0)
    {
        update(beg_element * sizeof(typename CONTAINER::value_type),
               data.size() * sizeof(typename CONTAINER::value_type),
               data.data());
    }
    template<typename T>
    void getData(void *buffer, unsigned nb_elements, unsigned beg_element = 0) const
    {
        bind();

        const T *ptr = static_cast<const T *>(
            glMapBufferRange(m_target, beg_element * sizeof(T), nb_elements * sizeof(T), GL_MAP_READ_BIT));

        std::copy(ptr, ptr + nb_elements, static_cast<T *>(buffer));

        glUnmapBuffer(m_target);

        unbind();
    }

protected:
    Model() = default;
    void destroy() override { cleanup(); }

private:
    void cleanup();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Index: public Model
{
private:
    unsigned m_N, m_M;

public:
    explicit Index(const std::vector<unsigned> &data, unsigned mode = GL_STATIC_DRAW);
    explicit Index(unsigned n, unsigned mode = GL_STATIC_DRAW);
    ~Index() override = default;
    Index(const Index &) = delete;
    Index(Index &&) = default;
    auto operator=(const Index &) -> Index & = delete;
    auto operator=(Index &&) -> Index & = default;
    void setNumberOfActiveIndices(unsigned m);
    [[nodiscard]] auto getNumberOfIndices() const -> unsigned { return m_N; }
    [[nodiscard]] auto getNumberOfActiveIndices() const -> unsigned { return m_M; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Vertex: public Model
{
public:
    template<typename T>
    class Entry
    {
    private:
        std::string m_name;
        T m_defaultValue;
        unsigned m_mode;

    public:
        explicit Entry(std::string name, T defaultValue = T(), unsigned mode = GL_STATIC_DRAW)
            : m_name(std::move(name)), m_defaultValue(defaultValue), m_mode(mode)
        {
        }
        [[nodiscard]] auto getName() const -> const std::string & { return m_name; }
        auto getDefaultValue() const -> const T & { return m_defaultValue; }
        [[nodiscard]] auto getMode() const -> unsigned { return m_mode; }
    };

private:
    unsigned m_type, m_N, m_D;

public:
    Vertex(unsigned n, unsigned d, unsigned s, void *data, unsigned type, unsigned mode);
    template<typename T>
    explicit Vertex(const std::vector<T> &data,
                    unsigned mode = GL_STATIC_DRAW,
                    typename std::enable_if<std::is_fundamental<T>::value>::type * /*unused*/ = nullptr)
        : Model(GL_ARRAY_BUFFER), m_type(getInternalType<T>()), m_N(data.size()), m_D(1)
    {
        glBindBuffer(GL_ARRAY_BUFFER, getId());
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), (void *) data.data(), mode);
    }
    template<typename T>
    explicit Vertex(const std::vector<T> &data,
                    unsigned mode = GL_STATIC_DRAW,
                    typename std::enable_if<!std::is_fundamental<T>::value>::type * /*unused*/ = nullptr)
        : Model(GL_ARRAY_BUFFER), m_type(getInternalType<typename T::value_type>()), m_N(data.size()), m_D(T::size())
    {
        glBindBuffer(GL_ARRAY_BUFFER, getId());
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), (void *) data.data(), mode);
    }
    ~Vertex() override = default;
    Vertex(const Vertex &) = delete;
    Vertex(Vertex &&) = default;
    auto operator=(const Vertex &) -> Vertex & = delete;
    auto operator=(Vertex &&) -> Vertex & = default;
    [[nodiscard]] auto getType() const -> unsigned { return m_type; }
    [[nodiscard]] auto getNumberOfElement() const -> unsigned { return m_N; }
    [[nodiscard]] auto getNumberOfCoordinatesPerElement() const -> unsigned { return m_D; }

private:
    template<typename T>
    auto getInternalType() -> unsigned;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Texture: public Model
{
public:
    Texture(): Model(GL_TEXTURE_BUFFER){};
    ~Texture() override = default;
    Texture(const Texture &) = delete;
    Texture(Texture &&) = default;
    auto operator=(const Texture &) -> Texture & = delete;
    auto operator=(Texture &&) -> Texture & = default;
    template<typename T>
    void reshape(unsigned n, unsigned mode = GL_STATIC_DRAW)
    {
        glBindBuffer(GL_TEXTURE_BUFFER, getId());
        glBufferData(GL_TEXTURE_BUFFER, n * sizeof(T), nullptr, mode);
    }
    template<typename T>
    void reshapeFromData(const T *data, std::size_t n, unsigned mode = GL_STATIC_DRAW)
    {
        glBindBuffer(GL_TEXTURE_BUFFER, getId());
        glBufferData(GL_TEXTURE_BUFFER, n * sizeof(T), data, mode);
    }
    template<typename CONTAINER>
    void reshapeFromData(const CONTAINER &data, unsigned mode = GL_STATIC_DRAW)
    {
        glBindBuffer(GL_TEXTURE_BUFFER, getId());
        glBufferData(GL_TEXTURE_BUFFER, data.size() * sizeof(typename CONTAINER::value_type), data.data(), mode);
    }
};
} // namespace Buffer
} // namespace iloj::gpu
