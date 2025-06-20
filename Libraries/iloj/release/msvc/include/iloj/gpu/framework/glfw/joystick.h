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

#include <iloj/gpu/interactor.h>
#include <iloj/misc/array.h>

namespace iloj::gpu::framework::glfw
{
class Joystick: public iloj::gpu::Interactor::Joystick
{
private:
    using AxisState = std::vector<float>;
    using ButtonState = std::vector<std::uint8_t>;
    using HatState = std::vector<std::uint8_t>;

private:
    int m_id{};
    std::string m_name;
    AxisState m_axis;
    ButtonState m_buttons;
    HatState m_hats;

public:
    Joystick(int id);
    [[nodiscard]] auto getId() const -> int { return m_id; }
    void update();
    [[nodiscard]] auto getName() const -> const std::string & override { return m_name; }
    [[nodiscard]] auto getNumberOfAxis() const -> std::size_t override { return m_axis.size(); }
    [[nodiscard]] auto getAxisState(std::size_t axisId = 0) const -> float override { return m_axis[axisId]; }
    [[nodiscard]] auto getNumberOfButtons() const -> std::size_t override { return m_buttons.size(); }
    [[nodiscard]] auto getButtonState(std::size_t buttonId = 0) const -> std::uint8_t override
    {
        return m_buttons[buttonId];
    }
    [[nodiscard]] auto getNumberOfHats() const -> std::size_t override { return m_hats.size(); }
    [[nodiscard]] auto getHatState(std::size_t hatId = 0) const -> std::uint8_t override { return m_hats[hatId]; }
};

} // namespace iloj::gpu::framework::glfw
