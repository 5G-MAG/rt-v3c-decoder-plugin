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

#include "string.h"

namespace iloj::misc
{
class Obfuscate
{
public:
    static auto encrypt(std::string key, std::string str) -> std::string;
    static auto decrypt(std::string key, std::string str) -> std::string;
};
} // namespace iloj::misc