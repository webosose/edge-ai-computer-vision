/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_LOGGER_H
#define AIF_LOGGER_H

#include <memory>
#include <mutex>
#include <cstring>

#include <aif/log/LogMessage.h>
#include <PmLogLib.h>

namespace aif {

class Logger
{
public:
    static void init(LogLevel loglevel = LogLevel::DEBUG);

//    static std::shared_ptr<Logger> get();
    static Logger& getInstance();
    static aif::LogLevel getLogLevel();
    static LogLevel strToLogLevel(const std::string& str);
    static std::string logLevelToStr(LogLevel level);
    static PmLogContext getPmLogContext();
    //Logger() = default;

    //~Logger();

    template<LogLevel loglevel, typename... Args>
    void printLog(const char* app, const char* ctx, const char* functioName, const char* fileName, int line, const Args&... args)
    {
        std::ostringstream msg;
        //msg << app << " " << ctx << " ";
        writer<loglevel>(functioName, fileName, line, msg, args...);
    }

    template<LogLevel loglevel, typename T, typename ... Types>
    void writer(const char* functioName, const char* fileName, int line,
                std::ostringstream& msg, T value, const Types&... args)
    {
        msg << value;
        writer<loglevel>(functioName, fileName, line, msg, args...);
    }

    template<LogLevel loglevel>
    void writer(const char* functioName, const char* fileName, int line, std::ostringstream& msg);


private:
    static std::unique_ptr<Logger> s_instance;
    static std::once_flag s_onceFlag;
    static aif::LogLevel s_logLevel;
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

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

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
