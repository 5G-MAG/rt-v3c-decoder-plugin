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

#include "descriptor.h"

namespace iloj::media
{
auto computePSNR(const Descriptor::Video &d1, const Descriptor::Video &d2, unsigned planeId = 0) -> double;
auto computeWSPSNR(const Descriptor::Video &d1, const Descriptor::Video &d2, unsigned planeId = 0) -> double;
} // namespace iloj::media
