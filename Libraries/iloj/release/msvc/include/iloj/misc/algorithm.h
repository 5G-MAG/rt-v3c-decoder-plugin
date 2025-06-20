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

#include <algorithm>

namespace iloj::misc
{
//! \brief Returns an iterator to the nth occurence between first and last satisfying closure
template<typename Iterator, typename Pred>
Iterator find_if_nth(Iterator first, Iterator last, Pred closure, int n)
{
    return std::find_if(first, last, [&](const auto &x) { return closure(x) && (0 == n--); });
}
} // namespace iloj::misc
