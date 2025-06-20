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

#include <common/video/pose.h>

using namespace iloj::math;

static Frame fOMAF{Direction::Forward, Direction::Leftward, Direction::Upward};
static Frame fGL{Direction::Rightward, Direction::Upward, Direction::Backward};

auto getOMAFtoGL() -> const Pose<float> &
{
    static auto OMAFtoGL = Pose<float>{Quaternion<float>::fromRotationMatrix(getRotationMatrix<float>(fOMAF, fGL)), {}};
    return OMAFtoGL;
}

auto getGLtoOMAF() -> const Pose<float> &
{
    static auto GLtoOMAF = Pose<float>{Quaternion<float>::fromRotationMatrix(getRotationMatrix<float>(fGL, fOMAF)), {}};
    return GLtoOMAF;
}

auto getGLPoseFromMivPose(const Pose<float> &p) -> Pose<float> { return p * getGLtoOMAF(); }

auto getGLPoseFromUnityPose(const iloj::math::Pose<float> &p) -> iloj::math::Pose<float>
{
    const auto &t = p.getTranslation();
    const auto &q = p.getQuaternion();

    return getGLtoOMAF() * Pose<float>{{q.x(), q.y(), -q.z(), -q.w()}, {t.x(), t.y(), -t.z()}};
}

auto getMivPoseFromUnityPose(const iloj::math::Pose<float> &p) -> iloj::math::Pose<float>
{
    return getGLPoseFromUnityPose(p) * getOMAFtoGL();
}

auto getAbsolutePoseFromGL(const Pose<float> &referenceGLToGlobalOMAF, const Pose<float> &localGLToReferenceOMAF)
    -> Pose<float>
{
    return referenceGLToGlobalOMAF * getOMAFtoGL() * localGLToReferenceOMAF;
}

auto getAbsolutePoseFromMiv(const Pose<float> &referenceOMAFToGlobalOMAF, const Pose<float> &localOMAFToReferenceOMAF)
    -> Pose<float>
{
    return referenceOMAFToGlobalOMAF * localOMAFToReferenceOMAF;
}