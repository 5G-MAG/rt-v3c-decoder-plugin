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

// NOLINTNEXTLINE
#define EXPAND(x) x
// NOLINTNEXTLINE
#define CONCAT2(x, y) x##y
// NOLINTNEXTLINE
#define CONCAT(x, y) CONCAT2(x, y)
// NOLINTNEXTLINE
#define STRINGIFY2(X) #X
// NOLINTNEXTLINE
#define STRINGIFY(X) STRINGIFY2(X)
// NOLINTNEXTLINE
#define MEQUAL(A, B) (((A / A) == 1) && ((B / B) == 1) && (A == B))
