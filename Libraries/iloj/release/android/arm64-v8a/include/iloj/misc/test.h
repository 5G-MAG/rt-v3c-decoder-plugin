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

#include <cstdlib>
#include <iloj/misc/logger.h>

// NOLINTNEXTLINE
#define TEST_SECTION(title) iloj::misc::Tester::getLogger().getStream() << "[TEST SECTION] " << (title) << '\n';

// NOLINTNEXTLINE
#define CHECK_ASSERTION(title, b)                                                                                      \
    if (b)                                                                                                             \
    {                                                                                                                  \
        iloj::misc::Tester::getLogger().append(                                                                        \
            iloj::misc::Logger::Level::Info, __FILE__, __LINE__, "\033[32mPASSED\033[0m", title);                      \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        iloj::misc::Tester::getLogger().append(                                                                        \
            iloj::misc::Logger::Level::Info, __FILE__, __LINE__, "\033[31mFAILED\033[0m", title);                      \
        exit(-1);                                                                                                      \
    }

namespace iloj::misc
{
class Tester
{
private:
    Logger m_logger;

private:
    Tester();

public:
    static auto getLogger() -> Logger &;
};
} // namespace iloj::misc
