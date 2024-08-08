/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <aif/tools/Utils.h>
#include <boost/exception/all.hpp>

#include <aif/log/LogMessage.h>

namespace aif {

std::mutex LogMessage::s_mutex;
bool LogMessage::s_writeToFile = false;
std::string LogMessage::s_logFileName;

LogMessage::LogMessage(LogLevel level)
    : std::ostream(&m_buffer)
    , m_level(level)
{
    m_when = boost::posix_time::microsec_clock::universal_time();
}

LogMessage::~LogMessage() noexcept
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

        try {
            if (s_writeToFile) {
                std::string reportContent = boost::posix_time::to_simple_string(m_when) + " [" + itsLevel + "] " + m_buffer.m_data.str() + "\n";
                strToFile(reportContent, s_logFileName);
            }
            else {
                std::cout << m_when << " [" << itsLevel << "] " << m_buffer.m_data.str() << std::endl;
            }
        } catch (...) {
            std::cout << boost::current_exception_diagnostic_information() << std::endl;
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

void LogMessage::setWriteToFile(bool writeToFile, const std::string& logFileName)
{
    s_writeToFile = writeToFile;
    s_logFileName = logFileName;
}

} // end of namespace aif


