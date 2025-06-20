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

#include "memory.h"
#include "string.h" // NOLINT
#include "thread.h"

#ifdef ENABLE_LOGGER

// NOLINTNEXTLINE
#define LOG_INFO(...)                                                                                                  \
    iloj::misc::Logger::getInstance().append(iloj::misc::Logger::Level::Info, __FILE__, __LINE__, "", __VA_ARGS__)

// NOLINTNEXTLINE
#define LOG_WARNING(...)                                                                                               \
    iloj::misc::Logger::getInstance().append(iloj::misc::Logger::Level::Warning, __FILE__, __LINE__, "", __VA_ARGS__)

// NOLINTNEXTLINE
#define LOG_ERROR(...)                                                                                                 \
    iloj::misc::Logger::getInstance().append(iloj::misc::Logger::Level::Error, __FILE__, __LINE__, "", __VA_ARGS__)

#ifdef NDEBUG

// NOLINTNEXTLINE
#define LOG_DEBUG(...)                                                                                                 \
    iloj::misc::Logger::getInstance().append(iloj::misc::Logger::Level::Debug, __FILE__, __LINE__, "", __VA_ARGS__)

#else

#define LOG_DEBUG(...)

#endif

#else

#define LOG_INFO(...)
#define LOG_WARNING(...)
#define LOG_ERROR(...)
#define LOG_DEBUG(...)

#endif

namespace iloj::misc
{
class Logger
{
public:
    struct Level
    {
        enum
        {
            Debug = 0,
            Info,
            Warning,
            Error
        };
    };
    struct Verbosity
    {
        enum
        {
            None = 0,
            Date = 1,
            Time = 2,
            File = 4,
            All = 7
        };
    };

private:
    Reference<std::ostream> m_os;
    unsigned m_level{Level::Warning};
    unsigned m_verbosity{Verbosity::All};
    unsigned m_pathMaxDepth{std::numeric_limits<unsigned>::max()};
    unsigned m_pathMaxLength{40};
    unsigned m_msgMaxLength{0};
    unsigned m_extraMaxLength{10};
    bool m_clearOnAppend = false;
    bool m_flushOnAppend = false;
    SpinLock m_pAccess;

public:
    explicit Logger(std::ostream &os = getDefaultStream()): m_os(os){};
    ~Logger() = default;
    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;
    auto operator=(const Logger &) -> Logger & = delete;
    auto operator=(Logger &&) -> Logger & = delete;
    void setStream(std::ostream &os) { m_os = os; }
    [[nodiscard]] auto getStream() const -> std::ostream & { return m_os.get(); }
    void setLevel(unsigned level) { m_level = level; }
    void setVerbosity(unsigned verbosity) { m_verbosity = verbosity; }
    void setPathMaxDepth(unsigned depth) { m_pathMaxDepth = depth; }
    void setPathMaxLength(unsigned length) { m_pathMaxLength = length; }
    void setMessageMaxLength(unsigned length) { m_msgMaxLength = length; }
    void setExtraMaxLength(unsigned length) { m_extraMaxLength = length; }
    void enableClearOnAppend(bool b) { m_clearOnAppend = b; }
    [[nodiscard]] auto isClearOnAppendEnabled() const -> bool { return m_clearOnAppend; }
    void enableFlushOnAppend(bool b) { m_flushOnAppend = b; }
    [[nodiscard]] auto isFlushOnAppendEnabled() const -> bool { return m_flushOnAppend; }
    void appendAtomic(unsigned level,
                      const std::string &file,
                      unsigned line,
                      const std::string &extra,
                      const std::string &msg);
    template<typename... ARGS>
    void append(unsigned level, const std::string &file, unsigned line, const std::string &extra, ARGS &&... next)
    {
        appendAtomic(level, file, line, extra, any2str(std::forward<ARGS>(next)...));
    }
    static auto getInstance() -> Logger &
    {
        static Logger singleton;
        return singleton;
    }
    static auto getStreamUID() -> std::string;
    static auto getDefaultStream() -> std::ostream &;
};
} // namespace iloj::misc
