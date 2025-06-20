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

#include "interactor.h"
#include "types.h"
#include <iloj/math/pose.h>
#include <set>

namespace iloj::gpu
{
class LookAt
{
public:
    class Interactor: public iloj::gpu::Interactor
    {
    public:
        enum class Motion
        {
            None,
            Rotation,
            Translation
        };

        enum KeyboardTranslationMapping
        {
            Leftward = 0,
            Rightward,
            Downward,
            Upward,
            Backward,
            Forward
        };

    protected:
        LookAt &m_lookAt;
        Motion m_currentMotion = Motion::None;
        iloj::math::Pose<float> m_currentPose;
        std::array<int, 2> m_lastPosition{};
        std::set<char> m_activeKey;
        MouseButton m_mouseRotationButton = MouseButton::Left, m_mouseTranslationButton = MouseButton::Right;
        unsigned short m_mouseForwardKey = 'z';
        float m_mouseTranslationScaling = 10.F, m_mouseRotationScaling = 5.F;
        std::array<unsigned short, 6> m_keyboardTranslationKeys = {'4', '6', '2', '8', '3', '9'};
        float m_keyboardTranslationStep = 0.01F;
        std::function<bool(LookAt &, unsigned short)> m_onKeyRelease;

    public:
        explicit Interactor(LookAt &lookAt): m_lookAt(lookAt) {}
        void setMouseTranslationScaling(float v) { m_mouseTranslationScaling = v; }
        [[nodiscard]] auto getMouseTranslationScaling() const -> float { return m_mouseTranslationScaling; }
        void setMouseRotationButton(MouseButton button) { m_mouseRotationButton = button; }
        void setMouseTranslationButton(MouseButton button) { m_mouseTranslationButton = button; }
        void setMouseForwardKey(unsigned short k) { m_mouseForwardKey = k; }
        void setMouseRotationScaling(float v) { m_mouseRotationScaling = v; }
        [[nodiscard]] auto getMouseRotationScaling() const -> float { return m_mouseRotationScaling; }
        void setKeyboardTranslationKeys(const std::array<unsigned short, 6> &keys) { m_keyboardTranslationKeys = keys; }
        void setKeyboardTranslationStep(float v) { m_keyboardTranslationStep = v; }
        [[nodiscard]] auto getKeyboardTranslationStep() const -> float { return m_keyboardTranslationStep; }

    public:
        void onMouseButtonPress(MouseButton button, int x, int y) override;
        void onMouseButtonRelease(MouseButton button, int x, int y) override;
        void onMouseMove(int x, int y) override;
        void onKeyPress(unsigned short key) override;
        void onKeyRelease(unsigned short key) override;
    };

private:
    std::array<Vec3f, 3> m_localDirection;
    iloj::math::Mat4x4f m_refToLocal;
    iloj::math::Pose<float> m_globalToLocal;
    iloj::math::Pose<float> m_localToGlobal;
    std::function<void()> m_onLookAtChange;

public:
    LookAt(const iloj::math::Frame &local);
    ~LookAt() = default;
    LookAt(const LookAt &) = default;
    LookAt(LookAt &&) = default;
    auto operator=(const LookAt &other) -> LookAt &;
    auto operator=(LookAt &&other) noexcept -> LookAt &;
    auto operator==(const LookAt &other) const -> bool { return (m_globalToLocal == other.m_globalToLocal); }
    auto operator!=(const LookAt &other) const -> bool { return !(*this == other); }
    [[nodiscard]] auto getLocalFrame() const -> iloj::math::Frame { return iloj::math::getFrame(m_localDirection); }
    [[nodiscard]] auto getForwardInLocalCoordinates() const -> const Vec3f & { return m_localDirection[0]; }
    [[nodiscard]] auto getLeftwardInLocalCoordinates() const -> const Vec3f & { return m_localDirection[1]; }
    [[nodiscard]] auto getUpwardInLocalCoordinates() const -> const Vec3f & { return m_localDirection[2]; }
    void setGlobalToLocal(const iloj::math::Pose<float> &pose);
    void setGlobalToLocal(const Vec3f &from, const Vec3f &to, const Vec3f &up);
    [[nodiscard]] auto getGlobalToLocal() const -> const iloj::math::Pose<float> & { return m_globalToLocal; }
    void setLocalToGlobal(const iloj::math::Pose<float> &pose);
    [[nodiscard]] auto getLocalToGlobal() const -> const iloj::math::Pose<float> & { return m_localToGlobal; }
    void setOnLookAtChangeCallback(const std::function<void()> &onLookAtChange) { m_onLookAtChange = onLookAtChange; }
};

} // namespace iloj::gpu
