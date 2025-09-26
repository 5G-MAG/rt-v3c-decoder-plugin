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

namespace iloj::gpu
{
class Attribute
{
private:
    int m_binding = -1;

public:
    Attribute(const std::string &name, unsigned program_id);
    ~Attribute() = default;
    Attribute(const Attribute &) = delete;
    Attribute(Attribute &&) = default;
    auto operator=(const Attribute &) -> Attribute & = delete;
    auto operator=(Attribute &&) -> Attribute & = default;
    [[nodiscard]] auto isValid() const -> bool { return (-1 != m_binding); }
    void setValue(Buffer::Vertex &array) const;
    static auto Default() -> Attribute &
    {
        static Attribute a;
        return a;
    }

protected:
    Attribute() = default;
};
} // namespace iloj::gpu
