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

#include <common/misc/types.h>
#include <iloj/math/matrix.h>


auto getFlipMode() -> unsigned;

struct Viewport
{
    unsigned width{};
    unsigned height{};
    unsigned left{};
    unsigned bottom{};
};

struct Job
{
    Viewport viewport{};
    TMIV::MivBitstream::ViewParams camera{};
    unsigned flipMode = getFlipMode();

    void updateViewport(unsigned w, unsigned h, unsigned left, unsigned bottom);
    void updateCameraProjection(unsigned typeId);
    void updateCameraResolution(unsigned w, unsigned h);
    void updateCameraIntrinsics(float k1, float k2, float k3, float k4);
    void updateCameraExtrinsics(float tx, float ty, float tz, float qx, float qy, float qz, float qw);
};

using JobList = std::vector<Job>;
