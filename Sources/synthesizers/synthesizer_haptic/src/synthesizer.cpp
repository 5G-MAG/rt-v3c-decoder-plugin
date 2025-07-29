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

#include <common/video/pose.h>
#include <iloj/gpu/options.h>
#include <iloj/misc/dll.h>
#include <iloj/misc/filesystem.h>
#include <iloj/misc/json.h>
#include <iloj/misc/logger.h>
#include <synthesizer.h>
#include <iostream>

using namespace iloj::math;
using namespace iloj::misc;

////////////////////////////////////////////////////////////////////////////////////////////////////
Synthesizer::Synthesizer(const std::string &configFile, unsigned synthesizerId)
{
}

void Synthesizer::synthesize(const int channelId,
                             const long duration,
                             const float startIntensity,
                             const float endIntensity)
{ 
    std::cout << "Haptic event [" << channelId << ": " << startIntensity << " -> " << endIntensity << "] : " << duration << "ms" << std::endl;
}
