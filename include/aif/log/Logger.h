/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_LOGGER_H
#define AIF_LOGGER_H

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>

#include <aif/log/LogMessage.h>
#include <PmLogLib.h>

namespace aif {

class Logger
{
public:
    static void init(LogLevel loglevel = LogLevel::INFO);

//    static std::shared_ptr<Logger> get();
    static Logger& getInstance();
    static aif::LogLevel getLogLevel();
    static LogLevel strToLogLevel(const std::string& str);
    static std::string logLevelToStr(LogLevel level);
    static PmLogContext getPmLogContext();
    static void setFullLog(bool fullLog);

    template<LogLevel loglevel, typename... Args>
    void printLog(const char* app, const char* ctx, const char* functioName, const char* fileName, int line, const Args&... args) noexcept
    {
        try {
            std::ostringstream msg;
            // msg << app << " " << ctx << " ";
            writer<loglevel>(functioName, fileName, line, msg, args...);
        } catch (const std::exception& e) {
            std::cerr << "Exception in printLog: " << e.what() << std::endl;
        }
    }

    template<LogLevel loglevel, typename T, typename ... Types>
    void writer(const char* functioName, const char* fileName, int line,
                std::ostringstream& msg, T value, const Types&... args) noexcept
    {
        try {
            msg << value;
            writer<loglevel>(functioName, fileName, line, msg, args...);
        } catch (const std::exception& e) {
            std::cerr << "Exception in writer: " << e.what() << std::endl;
        }
    }

    template<LogLevel loglevel>
    void writer(const char* functioName, const char* fileName, int line, std::ostringstream& msg) noexcept;


private:
    static std::once_flag s_onceFlag;
    static aif::LogLevel s_logLevel;
    static bool s_fullLog;
//    static std::mutex s_mutex;

    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};

} // end of namespace of aif

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

#ifndef LOG_APPLICATION
#define LOG_APPLICATION    "AIF"
#endif

#ifndef LOG_CONTEXT
#define LOG_CONTEXT        "AIF"
#endif

inline std::string truncateLength(const char* path, std::size_t length) noexcept {
    try {
        std::string fileName = std::filesystem::path(path).filename().string();
        return std::string(fileName.substr(0, std::min(fileName.size(), length)));
    } catch (const std::exception& e) {
        std::cerr << "Exception in truncateLength: " << e.what() << std::endl;
        return std::string(length, ' ');
    }
}

const std::size_t MAX_FILE_NAME_LENGTH = 31;
#define __FILENAME__ (truncateLength(__FILE__, MAX_FILE_NAME_LENGTH).c_str())

#if defined(ENABLE_DEBUG)

#define TRACE(...) aif::Logger::getInstance().printLog<aif::LogLevel::TRACE1>(LOG_APPLICATION, LOG_CONTEXT, \
        __func__, __FILENAME__, __LINE__, __VA_ARGS__)
#define TRACE2(...) aif::Logger::getInstance().printLog<aif::LogLevel::TRACE2>(LOG_APPLICATION, LOG_CONTEXT, \
        __func__, __FILENAME__, __LINE__, __VA_ARGS__)
#define TRACE3(...) aif::Logger::getInstance().printLog<aif::LogLevel::TRACE3>(LOG_APPLICATION, LOG_CONTEXT, \
        __func__, __FILENAME__, __LINE__, __VA_ARGS__)
#define TRACE4(...) aif::Logger::getInstance().printLog<aif::LogLevel::TRACE4>(LOG_APPLICATION, LOG_CONTEXT, \
        __func__, __FILENAME__, __LINE__, __VA_ARGS__)
#define Logv(...) aif::Logger::getInstance().printLog<aif::LogLevel::VERBOSE>(LOG_APPLICATION, LOG_CONTEXT, \
        __func__, __FILENAME__, __LINE__, __VA_ARGS__)
#define Logd(...) aif::Logger::getInstance().printLog<aif::LogLevel::DEBUG>(LOG_APPLICATION, LOG_CONTEXT, \
        __func__, __FILENAME__, __LINE__, __VA_ARGS__)
#define Logi(...) aif::Logger::getInstance().printLog<aif::LogLevel::INFO>(LOG_APPLICATION, LOG_CONTEXT, \
        __func__, __FILENAME__, __LINE__, __VA_ARGS__)
#define Logw(...) aif::Logger::getInstance().printLog<aif::LogLevel::WARNING>(LOG_APPLICATION, LOG_CONTEXT, \
        __func__,  __FILENAME__, __LINE__, __VA_ARGS__)
#define Loge(...) aif::Logger::getInstance().printLog<aif::LogLevel::ERROR>(LOG_APPLICATION, LOG_CONTEXT, \
        __func__,  __FILENAME__, __LINE__, __VA_ARGS__)
#define Logf(...) aif::Logger::getInstance().printLog<aif::LogLevel::FATAL>(LOG_APPLICATION, LOG_CONTEXT, \
        __func__,  __FILENAME__, __LINE__, __VA_ARGS__)
#else

#define TRACE(...)
#define TRACE2(...)
#define TRACE3(...)
#define TRACE4(...)
#define Logv(...)
#define Logd(...)
#define Logi(...)
#define Logw(...)
#define Loge(...) aif::Logger::getInstance().printLog<aif::LogLevel::ERROR>(LOG_APPLICATION, LOG_CONTEXT, \
        __func__,  __FILENAME__, __LINE__, __VA_ARGS__)
#define Logf(...) aif::Logger::getInstance().printLog<aif::LogLevel::FATAL>(LOG_APPLICATION, LOG_CONTEXT, \
        __func__,  __FILENAME__, __LINE__, __VA_ARGS__)

#endif

#endif // AIF_LOGGER_H
