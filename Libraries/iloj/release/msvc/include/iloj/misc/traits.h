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

#include <complex>
#include <type_traits>
#include <typeinfo>

// NOLINTNEXTLINE
#define has_type_macro(TYPE)                                                                                           \
    template<typename T>                                                                                               \
    struct has_type__##TYPE                                                                                            \
    {                                                                                                                  \
    private:                                                                                                           \
        using yes = std::uint8_t;                                                                                      \
        using no = std::uint16_t;                                                                                      \
        template<class U>                                                                                              \
        static auto test(typename U::TYPE *) -> yes;                                                                   \
        template<class U>                                                                                              \
        static auto test(...) -> no;                                                                                   \
                                                                                                                       \
    public:                                                                                                            \
        static const bool value = sizeof(test<T>(0)) == sizeof(yes);                                                   \
    };                                                                                                                 \
    template<typename T>                                                                                               \
    using has_##TYPE = has_type__##TYPE<T>;

namespace iloj::misc
{
//! \brief Check if template parameter is a numeric value (either real or complex)
template<typename T>
struct is_numeric
{
    static constexpr bool value = std::is_arithmetic<T>::value || std::is_same<T, std::complex<float>>::value ||
                                  std::is_same<T, std::complex<double>>::value;
};

//! \brief Check if template parameters are of same type
template<typename... Tn>
struct same_type
{
};

template<typename T1, typename T2>
struct same_type<T1, T2>
{
    static constexpr bool value = std::is_same<T1, T2>::value;
};

template<typename T1, typename T2, typename... Tn>
struct same_type<T1, T2, Tn...>
{
    static constexpr bool value = std::is_same<T1, T2>::value && same_type<T1, Tn...>::value;
};

} // namespace iloj::misc
