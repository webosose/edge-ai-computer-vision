#include <aif/log/Logger.h>

#include <iostream>
#include <thread>
#include <map>

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

template<>
void Logger::writer<LogLevel::FATAL>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    if (s_logLevel <  LogLevel::FATAL) return;
    AIF_FATAL << msg.str();
}

template<>
void Logger::writer<LogLevel::ERROR>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    if (s_logLevel <  LogLevel::ERROR) return;
    AIF_ERROR << msg.str();
}

template<>
void Logger::writer<LogLevel::WARNING>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    if (s_logLevel <  LogLevel::WARNING) return;
    AIF_WARNING << msg.str();
}

template<>
void Logger::writer<LogLevel::INFO>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    if (s_logLevel <  LogLevel::INFO) return;
    AIF_INFO << msg.str();
}

template<>
void Logger::writer<LogLevel::DEBUG>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    if (s_logLevel <  LogLevel::DEBUG) return;
    AIF_DEBUG << msg.str();
}

template<>
void Logger::writer<LogLevel::VERBOSE>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    if (s_logLevel <  LogLevel::VERBOSE) return;
    AIF_VERBOSE << msg.str();
}

template<>
void Logger::writer<LogLevel::TRACE1>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    if (s_logLevel <  LogLevel::TRACE1) return;
    AIF_TRACE1 << msg.str();
}

template<>
void Logger::writer<LogLevel::TRACE2>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    if (s_logLevel <  LogLevel::TRACE2) return;
    std::thread::id threadId = std::this_thread::get_id();
    AIF_TRACE2 << "(" << std::hex << threadId << ") " << std::dec << msg.str();
}

template<>
void Logger::writer<LogLevel::TRACE3>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    if (s_logLevel <  LogLevel::TRACE3) return;
    AIF_TRACE1 << msg.str() << " <at " << functionName << ":" << fileName << ":" << line << ">";
}

template<>
void Logger::writer<LogLevel::TRACE4>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    if (s_logLevel <  LogLevel::TRACE4) return;
    std::thread::id threadId = std::this_thread::get_id();
    AIF_TRACE2 << "(" << std::hex << threadId << ") " << std::dec << msg.str()
                << " <at " << functionName << ":" << fileName << ":" << line << ">";
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
