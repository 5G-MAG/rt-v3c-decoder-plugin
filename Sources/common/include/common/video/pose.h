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

#include <iloj/math/pose.h>

auto getGLPoseFromMivPose(const iloj::math::Pose<float> &p) -> iloj::math::Pose<float>;
auto getGLPoseFromUnityPose(const iloj::math::Pose<float> &p) -> iloj::math::Pose<float>;
auto getMivPoseFromUnityPose(const iloj::math::Pose<float> &p) -> iloj::math::Pose<float>;
auto getAbsolutePoseFromGL(const iloj::math::Pose<float> &globalOMAFToReferenceGL,
                           const iloj::math::Pose<float> &referenceOMAFToLocalGL) -> iloj::math::Pose<float>;
auto getAbsolutePoseFromMiv(const iloj::math::Pose<float> &referenceOMAFToGlobalOMAF,
                            const iloj::math::Pose<float> &localOMAFToReferenceOMAF) -> iloj::math::Pose<float>;