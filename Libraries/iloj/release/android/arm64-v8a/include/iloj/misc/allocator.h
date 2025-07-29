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

#include <cstddef>
#include <cstdint>

namespace iloj::misc
{
namespace Allocator
{
template<typename T>
class Aligned
{
public:
    using value_type = T;
    using pointer = T *;
    using const_pointer = const T *;
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::true_type;

private:
    std::size_t m_alignment{};

public:
    explicit Aligned(std::size_t alignment = alignof(std::max_align_t)): m_alignment(alignment) {}
    ~Aligned() = default;
    Aligned(const Aligned &other) = default;
    template<class U>
    Aligned(const Aligned<U> &other): m_alignment{other.getAligment()}
    {
    }
    Aligned(Aligned &&other) noexcept = default;
    auto operator=(const Aligned &) -> Aligned & = default;
    auto operator=(Aligned &&) noexcept -> Aligned & = default;
    [[nodiscard]] auto getAligment() const -> std::size_t { return m_alignment; }
    auto allocate(std::size_t n) -> T *
    {
        if (n != 0U)
        {
            std::size_t sz = n * sizeof(T);

            auto *mem = new std::uint8_t[sz + m_alignment + sizeof(std::size_t)]; // NOLINT
            auto *ptr = reinterpret_cast<std::size_t *>(
                (reinterpret_cast<std::size_t>(mem) + m_alignment + sizeof(std::size_t)) & ~(m_alignment - 1U));

            ptr[-1] = reinterpret_cast<std::size_t>(mem);

            return reinterpret_cast<T *>(ptr);
        }

        return nullptr;
    }
    void deallocate(T *ptr, std::size_t /*n*/)
    {
        // NOLINTNEXTLINE
        delete[] reinterpret_cast<std::uint8_t *>((reinterpret_cast<std::size_t *>(ptr))[-1]);
    }
};
} // namespace Allocator
} // namespace iloj::misc
