/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_LOG_MESSAGE_H
#define AIF_LOG_MESSAGE_H

#include <chrono>
#include <cstdint>
#include <mutex>
#include <ostream>
#include <sstream>
#include <streambuf>

namespace aif {

enum class LogLevel : std::uint8_t
{
    NONE = 0,
    FATAL = 1,
    ERROR = 2,
    WARNING = 3,
    INFO = 4,
    DEBUG = 5,
    VERBOSE = 6,
    TRACE1 = 7,
    TRACE2 = 8,
    TRACE3 = 9,
    TRACE4 = 10,
};

class LogMessage : public std::ostream
{
public:
    LogMessage(LogLevel level);
    ~LogMessage();

private:
    class Buffer : public std::streambuf
    {
    public:
        int_type overflow(int_type);
        std::streamsize xsputn(const char*, std::streamsize);

        std::stringstream m_data;
    };

    std::chrono::system_clock::time_point m_when;
    Buffer m_buffer;
    LogLevel m_level;
    static std::mutex s_mutex;
};

} // end of namespace aif

#define AIF_FATAL    aif::LogMessage(aif::LogLevel::FATAL)
#define AIF_ERROR    aif::LogMessage(aif::LogLevel::ERROR)
#define AIF_WARNING  aif::LogMessage(aif::LogLevel::WARNING)
#define AIF_INFO     aif::LogMessage(aif::LogLevel::INFO)
#define AIF_DEBUG    aif::LogMessage(aif::LogLevel::DEBUG)
#define AIF_VERBOSE  aif::LogMessage(aif::LogLevel::VERBOSE)
#define AIF_TRACE1   aif::LogMessage(aif::LogLevel::TRACE1)
#define AIF_TRACE2   aif::LogMessage(aif::LogLevel::TRACE2)
#define AIF_TRACE3   aif::LogMessage(aif::LogLevel::TRACE3)
#define AIF_TRACE4   aif::LogMessage(aif::LogLevel::TRACE4)

#endif // AIF_LOG_MESSAGE_H
