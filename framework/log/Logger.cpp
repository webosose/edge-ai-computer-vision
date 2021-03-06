#include <aif/log/Logger.h>

#include <iostream>
#include <thread>
#include <map>

#define PMLOG_CONTEXT_NAME "edgeai-vision"

namespace aif {

std::unique_ptr<Logger> Logger::s_instance;
std::once_flag Logger::s_onceFlag;
aif::LogLevel Logger::s_logLevel;
//std::mutex Logger::s_mutex;

void Logger::init(LogLevel loglevel)
{
    //std::lock_guard<std::mutex> itsLock(s_mutex);
    //auto itsLogger = Logger::getInstance();
    s_logLevel = loglevel;
    AIF_INFO << "Log Level: " + Logger::logLevelToStr(loglevel);

}

Logger& Logger::getInstance()
{
    std::call_once(Logger::s_onceFlag, []() {
        s_instance.reset(new Logger);
    });

    return *(s_instance.get());
}

LogLevel Logger::getLogLevel()
{
    return s_logLevel;
}

LogLevel Logger::strToLogLevel(const std::string& str)
{
    // TODO: fix to set automatically
    std::map<std::string, LogLevel> levelMap = {
        { "NONE", LogLevel::NONE },
        { "FATAL", LogLevel::FATAL },
        { "ERROR", LogLevel::ERROR },
        { "WARNING", LogLevel::WARNING },
        { "INFO", LogLevel::INFO },
        { "DEBUG", LogLevel::DEBUG },
        { "VERBOSE", LogLevel::VERBOSE },
        { "TRACE1", LogLevel::TRACE1 },
        { "TRACE2", LogLevel::TRACE2 },
        { "TRACE3", LogLevel::TRACE3 },
        { "TRACE4", LogLevel::TRACE4 }
    };
    if (levelMap.find(str) == levelMap.end())
        return LogLevel::DEBUG;

    return levelMap[str];
}

std::string Logger::logLevelToStr(LogLevel level)
{
    // TODO: fix to set automatically
    std::map<LogLevel, std::string> levelMap = {
        { LogLevel::NONE, "NONE" },
        { LogLevel::FATAL, "FATAL" },
        { LogLevel::ERROR, "ERROR" },
        { LogLevel::WARNING, "WARNING" },
        { LogLevel::INFO, "INFO" },
        { LogLevel::DEBUG, "DEBUG" },
        { LogLevel::VERBOSE, "VERBOSE" },
        { LogLevel::TRACE1, "TRACE1" },
        { LogLevel::TRACE2, "TRACE2" },
        { LogLevel::TRACE3, "TRACE3" },
        { LogLevel::TRACE4, "TRACE4" }
    };
    if (levelMap.find(level) == levelMap.end())
        return "DEBUG";

    return levelMap[level];
}

PmLogContext Logger::getPmLogContext() {
    static PmLogContext logContext = nullptr;
    PmLogErr logErr = kPmLogErr_None;
    if (logContext == nullptr) {
        PmLogGetContext(PMLOG_CONTEXT_NAME, &logContext);
    }
    return logContext;
}

template<>
void Logger::writer<LogLevel::FATAL>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    PmLogCritical(getPmLogContext(), fileName, 0, "[%s:%d] %s", functionName, line, msg.str().c_str());

#ifndef NDEBUG
    if (s_logLevel <  LogLevel::FATAL) return;
    AIF_FATAL << msg.str();
#endif
}

template<>
void Logger::writer<LogLevel::ERROR>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    PmLogError(getPmLogContext(), fileName, 0, "[%s:%d] %s", functionName, line, msg.str().c_str());

#ifndef NDEBUG
    if (s_logLevel <  LogLevel::ERROR) return;
    AIF_ERROR << msg.str();
#endif
}

template<>
void Logger::writer<LogLevel::WARNING>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    PmLogWarning(getPmLogContext(), fileName, 0, "[%s:%d] %s", functionName, line, msg.str().c_str());

#ifndef NDEBUG
    if (s_logLevel <  LogLevel::WARNING) return;
    AIF_WARNING << msg.str();
#endif
}

template<>
void Logger::writer<LogLevel::INFO>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    PmLogInfo(getPmLogContext(), fileName, 0, "[%s:%d] %s", functionName, line, msg.str().c_str());
#ifndef NDEBUG
    if (s_logLevel <  LogLevel::INFO) return;
    AIF_INFO << msg.str();
#endif
}

template<>
void Logger::writer<LogLevel::DEBUG>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    std::string logStr =
        std::string(fileName) +
        " [" + std::string(functionName) + ":" + std::to_string(line) + "] " +
        msg.str();
    PmLogDebug(getPmLogContext(), logStr.c_str());
#ifndef NDEBUG
    if (s_logLevel <  LogLevel::DEBUG) return;
    AIF_DEBUG << msg.str();
#endif
}

template<>
void Logger::writer<LogLevel::VERBOSE>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    PmLogDebug(getPmLogContext(), msg.str().c_str());
#ifndef NDEBUG
    if (s_logLevel <  LogLevel::VERBOSE) return;
    AIF_VERBOSE << msg.str();
#endif
}

template<>
void Logger::writer<LogLevel::TRACE1>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    PmLogDebug(getPmLogContext(), msg.str().c_str());
#ifndef NDEBUG
    if (s_logLevel <  LogLevel::TRACE1) return;
    AIF_TRACE1 << msg.str();
#endif
}

template<>
void Logger::writer<LogLevel::TRACE2>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    std::ostringstream ss;
    ss << std::this_thread::get_id();
    std::string logStr = "(tid:" + ss.str() + ")" + msg.str();
    PmLogDebug(getPmLogContext(), logStr.c_str());
#ifndef NDEBUG
    if (s_logLevel <  LogLevel::TRACE2) return;
    AIF_TRACE1 << logStr;
#endif
}

template<>
void Logger::writer<LogLevel::TRACE3>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    std::string logStr =
        std::string(fileName) +
        "[" + std::string(functionName) + ":" + std::to_string(line) + "]" +
        msg.str();
    PmLogDebug(getPmLogContext(), logStr.c_str());
#ifndef NDEBUG
    if (s_logLevel <  LogLevel::TRACE3) return;
    AIF_TRACE1 << logStr;
#endif
}

template<>
void Logger::writer<LogLevel::TRACE4>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    std::ostringstream ss;
    ss << std::this_thread::get_id();
    std::string logStr =
        std::string(fileName) +
        "[" + std::string(functionName) + ":" + std::to_string(line) + "]" +
        "(tid:" + ss.str() + ")" +
        msg.str();

    PmLogDebug(getPmLogContext(), logStr.c_str());
#ifndef NDEBUG
    if (s_logLevel <  LogLevel::TRACE4) return;
    AIF_TRACE1 << logStr;
#endif
}

#if 0
static std::shared_ptr<Logger>* s_loggerPtr(nullptr);
static std::mutex s_loggerMutex;

std::shared_ptr<Logger> Logger::get()
{
    std::lock_guard<std::mutex> itsLock(s_loggerMutex);

    if (s_loggerPtr == nullptr) {
        s_loggerPtr = new std::shared_ptr<Logger>();
    }

    if (s_loggerPtr != nullptr) {
        if (!(*s_loggerPtr)) {
            *s_loggerPtr = std::make_shared<Logger>();
        }
        return (*s_loggerPtr);
    }
    return (nullptr);
}

static void logger_impl_teardown(void) __attribute__((destructor));
static void logger_impl_teardown(void)
{
    if (s_loggerPtr != nullptr) {
        std::lock_guard<std::mutex> itsLock(s_loggerMutex);
        s_loggerPtr->reset();
        delete s_loggerPtr;
        s_loggerPtr = nullptr;
    }
}
#endif

} // end of namespace aif
