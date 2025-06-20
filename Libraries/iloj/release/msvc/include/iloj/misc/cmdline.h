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

#include <cstring>
#include <iloj/misc/string.h>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace iloj::misc
{
namespace CommandLine
{
//! \brief Base class implementing a commandline entry.
class Entry
{
protected:
    std::string m_tag;
    std::vector<std::string>::const_iterator m_iter;
    std::vector<std::string>::const_iterator m_last;

public:
    Entry(std::string tag, std::vector<std::string>::const_iterator iter, std::vector<std::string>::const_iterator last)
        : m_tag(std::move(tag)), m_iter(iter), m_last(last)
    {
    }
    virtual ~Entry() = default;
    Entry(const Entry &) = default;
    Entry(Entry &&) noexcept = default;
    auto operator=(const Entry &) -> Entry & = default;
    auto operator=(Entry &&) noexcept -> Entry & = default;
    //! \brief Returns entry tag name.
    [[nodiscard]] auto getTag() const -> const std::string & { return m_tag; }
    //! \brief Returns true if entry is valid.
    [[nodiscard]] virtual auto isValid() const -> bool = 0;
};

//! \brief Class implementing a commandline entry of type option (without any argument).
class Option: public Entry
{
public:
    using Entry::Entry;
    using Entry::operator=;
    [[nodiscard]] auto isValid() const -> bool override { return (m_iter != m_last); }
};

//! \brief Class implementing a commandline entry of type parameter (with one template argument).
template<typename T>
class Parameter: public Entry
{
private:
    T m_defaultValue{};

public:
    Parameter(const std::string &tag,
              std::vector<std::string>::const_iterator iter,
              std::vector<std::string>::const_iterator last,
              T defaultValue)
        : Entry(tag, iter, last), m_defaultValue{std::move(defaultValue)}
    {
    }
    ~Parameter() override = default;
    Parameter(const Parameter &) = default;
    Parameter(Parameter &&) noexcept = default;
    auto operator=(const Parameter &) -> Parameter & = default;
    auto operator=(Parameter &&) noexcept -> Parameter & = default;
    [[nodiscard]] auto isValid() const -> bool override { return (m_iter != m_last) && ((m_iter + 1) != m_last); }
    //! \brief Returns the parameter value or default value if invalid.
    [[nodiscard]] auto getValue() const -> T
    {
        if (isValid())
        {
            return str2any<T>(*(m_iter + 1));
        }

        return m_defaultValue;
    }
    //! \brief Returns the next parameter of the current command line with same name.
    [[nodiscard]] auto next() const -> Parameter
    {
        return {m_tag, std::find(m_iter + 1, m_last, m_tag), m_last, m_defaultValue};
    }
};

//! \brief Class implementing a commandline entry of type keyvalue (with one pair of string / template arguments).
class KeyValue: public Entry
{
public:
    using Entry::Entry;
    using Entry::operator=;
    [[nodiscard]] auto isValid() const -> bool override
    {
        return (m_iter != m_last) && ((m_iter + 1) != m_last) && ((m_iter + 2) != m_last);
    }
    //! \brief Returns the key name.
    [[nodiscard]] auto getKey() const -> std::string
    {
        if (isValid())
        {
            return *(m_iter + 1);
        }

        return {};
    }
    //! \brief Returns the key value.
    template<typename T>
    [[nodiscard]] auto getValue() const -> T
    {
        if (isValid())
        {
            return str2any<T>(*(m_iter + 2));
        }

        return {};
    }
    //! \brief Returns the next keyvalue of the current command line with same name.
    [[nodiscard]] auto next() const -> KeyValue { return {m_tag, std::find(m_iter + 1, m_last, m_tag), m_last}; }
};

class Parser
{
private:
    std::vector<std::string> m_argv;
    std::string m_commandName;

public:
    Parser(int argc, const char **argv): m_argv(argv + 1, argv + argc), m_commandName(argv[0]) {}
    Parser(const Parser &) = default;
    Parser(Parser &&) = default;
    ~Parser() = default;
    auto operator=(const Parser &) -> Parser & = default;
    auto operator=(Parser &&) -> Parser & = default;
    [[nodiscard]] auto getArguments() const -> const std::vector<std::string> & { return m_argv; }
    [[nodiscard]] auto getCommandName() const -> const std::string & { return m_commandName; }
    [[nodiscard]] auto getCommandLine() const -> std::string
    {
        std::ostringstream oss;

        oss << m_commandName;

        for (const auto &k : m_argv)
        {
            oss << ' ' << k;
        }

        return oss.str();
    }
    //! \brief Returns true is the requested option is specified in the command line.
    [[nodiscard]] auto hasOption(const std::string &tag) const -> bool
    {
        return Option{tag, std::find(m_argv.begin(), m_argv.end(), tag), m_argv.end()}.isValid();
    }
    //! \brief Returns the first occurence of the requested parameter in the command line.
    template<typename T>
    [[nodiscard]] auto getParameter(const std::string &tag, T defaultValue = {}) const -> Parameter<T>
    {
        return Parameter<T>{tag, std::find(m_argv.begin(), m_argv.end(), tag), m_argv.end(), std::move(defaultValue)};
    }
    //! \brief Returns the first occurence of the requested keyvalue in the command line.
    [[nodiscard]] auto getKeyValue(const std::string &tag) const -> KeyValue
    {
        return KeyValue{tag, std::find(m_argv.begin(), m_argv.end(), tag), m_argv.end()};
    }
};
} // namespace CommandLine
} // namespace iloj::misc
