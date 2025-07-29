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

#include <algorithm>
#include <cstdio>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace iloj::misc
{
//! \brief C-like string format function
template<class... Args>
auto format(char const *fmt, Args &&... args) -> std::string
{
    auto chars = snprintf(nullptr, 0, fmt, std::forward<Args>(args)...); // NOLINT
    std::vector<char> buffer(chars + 1);
    snprintf(buffer.data(), chars + 1, fmt, std::forward<Args>(args)...); // NOLINT
    return {buffer.begin(), buffer.end() - 1};
}

//! \brief Splits input string according to given regex.
inline auto split(const std::string &s, const std::string &regex) -> std::vector<std::string>
{
    std::regex re(regex);
    std::sregex_token_iterator first{s.begin(), s.end(), re, -1};
    std::sregex_token_iterator last;
    std::vector<std::string> v = {first, last};
    v.erase(std::remove_if(v.begin(), v.end(), [](const std::string &ss) { return ss.empty(); }), v.end());
    return v;
}

//! \brief Replace all occurences of fromto[i].first into fromto[i].second and returns.
auto replace(const std::string &input,
             const std::vector<std::pair<std::string, std::string>> &fromto,
             bool first = false) -> std::string;

//! \brief Counts number of occurences of sample within input.
auto count(const std::string &input, const std::string &sample) -> int;

//! \brief Repeats n times and returns.
auto repeat(const std::string &s, unsigned n) -> std::string;

//! \brief Shortens the string if it exceeds max_size, keeping first and last part as well as replacing characters extra
//! by the provided wildcard.
auto shorten(const std::string &s, unsigned maxSize, char wildcard = '.', unsigned nbWildcard = 4) -> std::string;

//! \brief Converts input string to required template type.
template<typename T>
auto str2any(const std::string &s) -> T
{
    T out;

    std::istringstream is(s);
    is >> out;

    return out;
}

template<>
inline auto str2any<std::string>(const std::string &s) -> std::string
{
    return s;
}

//! \brief Checks if string is a number
auto isNumber(const std::string &s) -> bool;

//! \brief Appends variadic template arguments to stream.
inline void appendToStream(std::ostream & /*os*/) {}

template<typename T>
void appendAtomicToStream(std::ostream &os, const T &v)
{
    os << v; // NOLINT
}

template<typename T>
void appendToStream(std::ostream &os, const std::vector<T> &buffer)
{
    for (const auto &v : buffer)
    {
        appendAtomicToStream(os, v);
        os << " ";
    }
}

template<typename T, typename... ARGS>
void appendToStream(std::ostream &os, const T &first, ARGS &&... next)
{
    appendAtomicToStream(os, first);
    appendToStream(os, std::forward<ARGS>(next)...);
}

//! \brief Concatenates variadic template arguments into one string.
template<typename... ARGS>
auto any2str(ARGS &&... next) -> std::string
{
    std::ostringstream os;

    appendToStream(os, std::forward<ARGS>(next)...);

    return os.str();
}

//! \brief Converts to a vector of template type separated per given regex in input string.
template<typename T>
auto str2vec(const std::string &s, const std::string &regex = "\\;") -> std::vector<T>
{
    std::regex re(regex);
    std::sregex_token_iterator iter{s.begin(), s.end(), re, -1};
    std::sregex_token_iterator last;
    std::vector<T> result;

    for (; iter != last; ++iter)
    {
        result.push_back(str2any<T>(iter->str()));
    }

    return result;
}

//! \brief Converts to a range of values of template type specified per given separator regex and range regex in input
//! string.
template<typename T>
auto str2range(const std::string &s,
               const std::string &regex_sep = "\\,",
               const std::string &regex_range = "\\:",
               T default_step = T(1)) -> std::vector<T>
{
    std::vector<T> out;
    std::vector<std::string> groups = split(s, regex_sep);

    for (auto &g : groups)
    {
        std::vector<std::string> elements = split(g, regex_range);

        if (elements.size() == 1)
        {
            out.push_back(str2any<T>(elements[0]));
        }
        else if (elements.size() == 2)
        {
            T v0 = str2any<T>(elements[0]);
            T v1 = str2any<T>(elements[1]);

            for (T w = v0; w <= v1; w += default_step)
            {
                out.push_back(w);
            }
        }
        else if (elements.size() == 3)
        {
            T v0 = str2any<T>(elements[0]);
            T v1 = str2any<T>(elements[2]);
            T step = str2any<T>(elements[1]);

            for (T w = v0; w <= v1; w += step)
            {
                out.push_back(w);
            }
        }
    }

    return out;
}

//! \brief Converts input string to a unique identifier (hash table).
auto str2uid(const std::string &s) -> std::size_t;

//! \brief Converts input integer to string with given width and sign presence.
auto int2str(int value, unsigned width, bool sign = false) -> std::string;

//! \brief Converts input integer to float with given width, decimal and sign presence.
auto float2str(double value, unsigned width, unsigned decimal, bool sign = false, double eps = 1e-7) -> std::string;

//! \brief Converts a binary array to a string (ascii85).
auto bin2str(const std::uint8_t *buffer, std::size_t buffer_size) -> std::string;

//! \brief Converts input string to a binary array (ascii85).
auto str2bin(const std::string &s, std::uint8_t *buffer, std::size_t buffer_size) -> int;

//! \brief Returns a thread-safe ramdom alpha numeric string of size length.
auto random_string(std::size_t length) -> std::string;

//! \brief Converts input string to lower case.
auto to_lower(const std::string &s) -> std::string;

//! \brief Converts input string to upper case.
auto to_upper(const std::string &s) -> std::string;

} // namespace iloj::misc
