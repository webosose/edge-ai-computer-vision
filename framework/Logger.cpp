#include <aif/Logger.h>

#include <iostream>
#include <thread>

namespace aif {

std::unique_ptr<Logger> Logger::s_instance;
std::once_flag Logger::s_onceFlag;
//std::mutex Logger::s_mutex;

void Logger::init()
{
    //std::lock_guard<std::mutex> itsLock(s_mutex);
    //auto itsLogger = Logger::getInstance();
}

Logger& Logger::getInstance()
{
    std::call_once(Logger::s_onceFlag, []() {
        s_instance.reset(new Logger);
    });

    return *(s_instance.get());
}

template<>
void Logger::writer<LogLevel::FATAL>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    AIF_FATAL << msg.str();
}

template<>
void Logger::writer<LogLevel::ERROR>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    AIF_ERROR << msg.str();
}

template<>
void Logger::writer<LogLevel::WARNING>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    AIF_WARNING << msg.str();
}

template<>
void Logger::writer<LogLevel::INFO>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    AIF_INFO << msg.str();
}

template<>
void Logger::writer<LogLevel::DEBUG>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    AIF_DEBUG << msg.str();
}

template<>
void Logger::writer<LogLevel::VERBOSE>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    AIF_VERBOSE << msg.str();
}

template<>
void Logger::writer<LogLevel::TRACE1>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    AIF_TRACE1 << msg.str();
}

template<>
void Logger::writer<LogLevel::TRACE2>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    std::thread::id threadId = std::this_thread::get_id();
    AIF_TRACE2 << "(" << std::hex << threadId << ") " << std::dec << msg.str();
}

template<>
void Logger::writer<LogLevel::TRACE3>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
    AIF_TRACE1 << msg.str() << " <at " << functionName << ":" << fileName << ":" << line << ">";
}

template<>
void Logger::writer<LogLevel::TRACE4>(const char* functionName, const char* fileName, int line, std::ostringstream& msg)
{
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
