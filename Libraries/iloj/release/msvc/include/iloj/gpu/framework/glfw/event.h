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

namespace iloj::gpu::framework::glfw
{
class Event
{
private:
    unsigned m_id = 0;
    void *m_data = nullptr;

public:
    explicit Event(unsigned id, void *data = nullptr): m_id(id), m_data(data) {}
    [[nodiscard]] auto getId() const -> unsigned { return m_id; }
    auto getData() -> void * { return m_data; }
    [[nodiscard]] auto getData() const -> const void * { return m_data; }
};
} // namespace iloj::gpu::framework::glfw
