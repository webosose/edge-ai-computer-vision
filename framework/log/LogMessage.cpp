/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>

#include <aif/log/LogMessage.h>

namespace aif {

std::mutex LogMessage::s_mutex;

LogMessage::LogMessage(LogLevel level)
    : std::ostream(&m_buffer)
    , m_level(level)
{
    m_when = std::chrono::system_clock::now();
}

LogMessage::~LogMessage()
{
    std::lock_guard<std::mutex> itsLock(s_mutex);

    {
        // Prepare log level
        const char *itsLevel;
        switch (m_level) {
        case LogLevel::FATAL:
            itsLevel = "F";
            break;
        case LogLevel::ERROR:
            itsLevel = "E";
            break;
        case LogLevel::WARNING:
            itsLevel = "W";
            break;
        case LogLevel::INFO:
            itsLevel = "I";
            break;
        case LogLevel::DEBUG:
            itsLevel = "D";
            break;
        case LogLevel::VERBOSE:
            itsLevel = "V";
            break;
        case LogLevel::TRACE1:
        case LogLevel::TRACE2:
        case LogLevel::TRACE3:
        case LogLevel::TRACE4:
            itsLevel = "T";
            break;
        default:
            itsLevel = "none";
        };

        // Prepare time stamp
        auto its_time_t = std::chrono::system_clock::to_time_t(m_when);
        auto its_time = std::localtime(&its_time_t);
        auto its_ms = (m_when.time_since_epoch().count() / 100) % 1000000;

        // CID9333389, CID9333366
        if (its_time != nullptr) {
            std::cout
                << std::dec << std::setw(4) << its_time->tm_year + 1900 << "-"
                << std::dec << std::setw(2) << std::setfill('0') << its_time->tm_mon << "-"
                << std::dec << std::setw(2) << std::setfill('0') << its_time->tm_mday << " "
                << std::dec << std::setw(2) << std::setfill('0') << its_time->tm_hour << ":"
                << std::dec << std::setw(2) << std::setfill('0') << its_time->tm_min << ":"
                << std::dec << std::setw(2) << std::setfill('0') << its_time->tm_sec << "."
                << std::dec << std::setw(6) << std::setfill('0') << its_ms << " ["
                << itsLevel << "] "
                << m_buffer.m_data.str()
                << std::endl;
        }
    }
}

std::streambuf::int_type
LogMessage::Buffer::overflow(std::streambuf::int_type c)
{
    if (c != EOF) {
        m_data << (char)c;
    }

    return (c);
}

std::streamsize
LogMessage::Buffer::xsputn(const char *s, std::streamsize n)
{
    m_data.write(s, n);
    return (n);
}

} // end of namespace aif


