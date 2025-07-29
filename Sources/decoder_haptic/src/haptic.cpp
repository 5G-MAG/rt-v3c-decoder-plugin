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

#include <fstream>
#include <iloj/misc/filesystem.h>
#include <iloj/misc/logger.h>
#include <decoder_haptic.h>
#include <types.h>
#include <iostream>

static std::unique_ptr<DecoderHaptic> g_decoderhaptic;
extern "C" INTERFACE_EXPORT void INTERFACE_API init(std::chrono::duration<double> initTime)
{
    g_decoderhaptic = std::make_unique<DecoderHaptic>(initTime);
}

extern "C" INTERFACE_EXPORT void INTERFACE_API decode(std::string s, HapticInput& hapticInput)
{ 
    g_decoderhaptic->decode(s, hapticInput);
}
