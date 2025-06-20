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

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

namespace iloj::misc
{
//! \brief Reference class implementing a possibly uninitialized managed reference.
template<typename T>
class Reference
{
private:
    std::unique_ptr<std::reference_wrapper<T>> m_data{};

public:
    //! \brief Default constructor.
    Reference() = default;
    //! \brief Default destructor.
    ~Reference() = default;
    //! \brief Copy constructor.
    Reference(const Reference &other)
    {
        if (other.m_data)
        {
            m_data = std::make_unique<std::reference_wrapper<T>>(*other.m_data);
        }
    }
    //! \brief Move constructor.
    Reference(Reference &&) noexcept = default;
    //! \brief Constructor from explicit reference.
    Reference(T &object): m_data(std::make_unique<std::reference_wrapper<T>>(object)) {}
    //! \brief Copy assignment.
    auto operator=(const Reference &other) -> Reference &
    {
        if (other.m_data)
        {
            m_data = std::make_unique<std::reference_wrapper<T>>(*other.m_data);
        }
        else
        {
            m_data.reset();
        }

        return *this;
    }
    //! \brief Move assignment.
    auto operator=(Reference &&other) noexcept -> Reference & = default;
    //! \brief Assignement from explicit reference
    auto operator=(T &object) -> Reference &
    {
        m_data = std::make_unique<std::reference_wrapper<T>>(object);
        return *this;
    }
    //! \brief Returns true if the managed reference is initialized.
    [[nodiscard]] auto isValid() const -> bool { return (m_data != nullptr); }
    //! \brief Returns the managed reference when valid.
    [[nodiscard]] auto get() const -> T & { return m_data->get(); }
    //! \brief Dereferences pointer to the managed reference.
    auto operator->() const -> T * { return std::addressof(m_data->get()); }
    //! \brief Return data pointer.
    [[nodiscard]] auto data() const -> T * { return m_data ? std::addressof(m_data->get()) : nullptr; }
    //! \brief Clears current managed reference.
    void clear() { m_data.reset(); }
};

//! \brief Simple class implementing a naive input binary stream from external buffer
class InputStream
{
private:
    const std::uint8_t *const m_buffer = nullptr;
    std::size_t m_size{}, m_pos{};

public:
    InputStream(const std::uint8_t *const buffer, std::size_t sz): m_buffer{buffer}, m_size{sz} {}
    ~InputStream() = default;
    InputStream(const InputStream &) = delete;
    InputStream(InputStream &&) = delete;
    auto operator=(const InputStream &) -> InputStream & = delete;
    auto operator=(InputStream &&) -> InputStream & = delete;
    [[nodiscard]] auto data() const -> const std::uint8_t * { return m_buffer; }
    [[nodiscard]] auto empty() const -> bool { return (m_size == 0); }
    [[nodiscard]] auto available() const -> std::size_t { return (m_size - m_pos); }
    template<typename T>
    auto read() -> T
    {
        auto v = *reinterpret_cast<const T *>(m_buffer + m_pos); // NOLINT
        m_pos += sizeof(T);
        return v;
    }
    auto read(std::uint8_t *const buffer, std::size_t buffer_size) -> std::size_t
    {
        std::size_t bytes = std::min(buffer_size, available());

        std::copy(m_buffer + m_pos, m_buffer + (m_pos + bytes), buffer);
        m_pos += bytes;

        return bytes;
    }
    template<typename T>
    auto peek() -> T
    {
        auto pos = m_pos;
        auto v = read<T>();
        m_pos = pos;
        return v;
    }
    template<typename T>
    auto pop() -> T
    {
        T out{};
        *this >> out;
        return out;
    }
    void seekg(std::size_t pos) { m_pos = pos; }
    [[nodiscard]] auto tellg() const -> std::size_t { return m_pos; }
    void clear() { m_pos = 0; }
};

//! \brief Simple class implementing a naive output binary stream from external buffer
class OutputStream
{
private:
    std::uint8_t *const m_buffer = nullptr;
    std::size_t m_size{}, m_pos{};

public:
    OutputStream(std::uint8_t *const buffer, std::size_t sz): m_buffer{buffer}, m_size{sz} {}
    ~OutputStream() = default;
    OutputStream(const OutputStream &) = delete;
    OutputStream(OutputStream &&) = delete;
    auto operator=(const OutputStream &) -> OutputStream & = delete;
    auto operator=(OutputStream &&) -> OutputStream & = delete;
    [[nodiscard]] auto data() const -> const std::uint8_t * { return m_buffer; }
    [[nodiscard]] auto empty() const -> bool { return (m_size == 0); }
    [[nodiscard]] auto available() const -> std::size_t { return (m_size - m_pos); }
    template<typename T>
    void write(T v)
    {
        *reinterpret_cast<T *>(m_buffer + m_pos) = v; // NOLINT
        m_pos += sizeof(T);
    }
    auto write(const std::uint8_t *const buffer, std::size_t buffer_size) -> std::size_t
    {
        std::size_t bytes = std::min(buffer_size, available());

        std::copy(buffer, buffer + bytes, m_buffer + m_pos);
        m_pos += bytes;

        return bytes;
    }
    template<typename T>
    void push(const T &v)
    {
        *this << v;
    }
    void seekp(std::size_t pos) { m_pos = pos; }
    [[nodiscard]] auto tellp() const -> std::size_t { return m_pos; }
    void clear() { m_pos = 0; }
};

} // namespace iloj::misc

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline auto operator>>(iloj::misc::InputStream &is, bool &v) -> iloj::misc::InputStream &
{
    v = static_cast<bool>(is.read<std::uint8_t>());
    return is;
}

inline auto operator>>(iloj::misc::InputStream &is, std::int8_t &v) -> iloj::misc::InputStream &
{
    v = is.read<std::int8_t>();
    return is;
}

inline auto operator>>(iloj::misc::InputStream &is, std::int16_t &v) -> iloj::misc::InputStream &
{
    v = is.read<std::int16_t>();
    return is;
}

inline auto operator>>(iloj::misc::InputStream &is, std::int32_t &v) -> iloj::misc::InputStream &
{
    v = is.read<std::int32_t>();
    return is;
}

inline auto operator>>(iloj::misc::InputStream &is, std::int64_t &v) -> iloj::misc::InputStream &
{
    v = is.read<std::int64_t>();
    return is;
}

inline auto operator>>(iloj::misc::InputStream &is, std::uint8_t &v) -> iloj::misc::InputStream &
{
    v = is.read<std::uint8_t>();
    return is;
}

inline auto operator>>(iloj::misc::InputStream &is, std::uint16_t &v) -> iloj::misc::InputStream &
{
    v = is.read<std::uint16_t>();
    return is;
}

inline auto operator>>(iloj::misc::InputStream &is, std::uint32_t &v) -> iloj::misc::InputStream &
{
    v = is.read<std::uint32_t>();
    return is;
}

inline auto operator>>(iloj::misc::InputStream &is, std::uint64_t &v) -> iloj::misc::InputStream &
{
    v = is.read<std::uint64_t>();
    return is;
}

inline auto operator>>(iloj::misc::InputStream &is, float &v) -> iloj::misc::InputStream &
{
    v = is.read<float>();
    return is;
}

inline auto operator>>(iloj::misc::InputStream &is, double &v) -> iloj::misc::InputStream &
{
    v = is.read<double>();
    return is;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline auto operator<<(iloj::misc::OutputStream &os, bool v) -> iloj::misc::OutputStream &
{
    os.write<std::uint8_t>(static_cast<std::uint8_t>(v));
    return os;
    ;
}

inline auto operator<<(iloj::misc::OutputStream &os, const std::int8_t &v) -> iloj::misc::OutputStream &
{
    os.write<std::uint8_t>(v);
    return os;
}

inline auto operator<<(iloj::misc::OutputStream &os, std::int16_t v) -> iloj::misc::OutputStream &
{
    os.write<std::int16_t>(v);
    return os;
}

inline auto operator<<(iloj::misc::OutputStream &os, std::int32_t v) -> iloj::misc::OutputStream &
{
    os.write<std::int32_t>(v);
    return os;
}

inline auto operator<<(iloj::misc::OutputStream &os, std::int64_t v) -> iloj::misc::OutputStream &
{
    os.write<std::int64_t>(v);
    return os;
}

inline auto operator<<(iloj::misc::OutputStream &os, std::uint8_t v) -> iloj::misc::OutputStream &
{
    os.write<std::uint8_t>(v);
    return os;
}

inline auto operator<<(iloj::misc::OutputStream &os, std::uint16_t v) -> iloj::misc::OutputStream &
{
    os.write<std::uint16_t>(v);
    return os;
}

inline auto operator<<(iloj::misc::OutputStream &os, std::uint32_t v) -> iloj::misc::OutputStream &
{
    os.write<std::uint32_t>(v);
    return os;
}

inline auto operator<<(iloj::misc::OutputStream &os, std::uint64_t v) -> iloj::misc::OutputStream &
{
    os.write<std::uint64_t>(v);
    return os;
}

inline auto operator<<(iloj::misc::OutputStream &os, float v) -> iloj::misc::OutputStream &
{
    os.write<float>(v);
    return os;
}

inline auto operator<<(iloj::misc::OutputStream &os, double v) -> iloj::misc::OutputStream &
{
    os.write<double>(v);
    return os;
}

// std::array specialization
template<typename T, std::size_t N>
auto operator>>(iloj::misc::InputStream &is, std::array<T, N> &a) -> iloj::misc::InputStream &
{
    for (auto &v : a)
    {
        is >> v;
    }

    return is;
}

template<typename T, std::size_t N>
auto operator<<(iloj::misc::OutputStream &os, const std::array<T, N> &a) -> iloj::misc::OutputStream &
{
    for (auto &v : a)
    {
        os << v;
    }

    return os;
}

// std::vector specialization
template<typename V, std::enable_if_t<std::is_base_of<std::vector<typename V::value_type>, V>::value, int> = 0>
auto operator>>(iloj::misc::InputStream &is, V &a) -> iloj::misc::InputStream &
{
    a.resize(is.read<std::size_t>());

    for (auto &v : a)
    {
        is >> v;
    }

    return is;
}

template<typename V, std::enable_if_t<std::is_base_of<std::vector<typename V::value_type>, V>::value, int> = 0>
auto operator<<(iloj::misc::OutputStream &os, const V &a) -> iloj::misc::OutputStream &
{
    os << a.size();

    for (const auto &v : a)
    {
        os << v;
    }

    return os;
}
