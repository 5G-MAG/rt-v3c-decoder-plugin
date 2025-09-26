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

#include "mesh.h"

namespace iloj::gpu
{
auto createMeshFromPLY(const std::string &path, unsigned attributes) -> Mesh;
void exportPLYFromMesh(const std::string &path, const Mesh &mesh, bool isAscii = false);
} // namespace iloj::gpu
