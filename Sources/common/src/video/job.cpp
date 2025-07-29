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

#include <common/video/job.h>

void Job::updateViewport(unsigned w, unsigned h, unsigned left, unsigned bottom) { viewport = {w, h, left, bottom}; }

void Job::updateCameraProjection(unsigned typeId)
{
    camera.ci.ci_cam_type(static_cast<TMIV::MivBitstream::CiCamType>(typeId));
}

void Job::updateCameraResolution(unsigned w, unsigned h)
{
    camera.ci.ci_projection_plane_width_minus1(static_cast<std::int32_t>(w) - 1);
    camera.ci.ci_projection_plane_height_minus1(static_cast<std::int32_t>(h) - 1);
}

void Job::updateCameraIntrinsics(float k1, float k2, float k3, float k4)
{
    switch (camera.ci.ci_cam_type())
    {
        case TMIV::MivBitstream::CiCamType::equirectangular:
        {
            camera.ci.ci_erp_phi_min(k1);
            camera.ci.ci_erp_phi_max(k2);
            camera.ci.ci_erp_theta_min(k3);
            camera.ci.ci_erp_theta_max(k4);

            break;
        }
        case TMIV::MivBitstream::CiCamType::perspective:
        {
            camera.ci.ci_perspective_focal_hor(k1);
            camera.ci.ci_perspective_focal_ver(k2);
            camera.ci.ci_perspective_center_hor(k3);
            camera.ci.ci_perspective_center_ver(k4);

            break;
        }
        case TMIV::MivBitstream::CiCamType::orthographic:
        {
            camera.ci.ci_ortho_width(k1);
            camera.ci.ci_ortho_height(k2);

            break;
        }
    }
}

void Job::updateCameraExtrinsics(float tx, float ty, float tz, float qx, float qy, float qz, float qw)
{
    camera.pose.position = {tx, ty, tz};
    camera.pose.orientation = {qx, qy, qz, qw};
}
