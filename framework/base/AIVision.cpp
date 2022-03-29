#include <aif/base/AIVision.h>
#include <aif/log/Logger.h>

#define KEY_LOG_LEVEL "LogLevel"
#define KEY_BASE_PATH "BasePath"

namespace aif {

bool AIVision::s_initialized = false;
std::unique_ptr<ConfigReader> AIVision::s_config;

void AIVision::init()
{
    if (s_initialized)
        return;

    s_config = std::make_unique<ConfigReader>(
            std::string(EDGEAI_VISION_HOME) + "/" + std::string(EDGEAI_VISION_CONFIG));
    s_initialized = true;
    Logger::init(Logger::strToLogLevel(s_config->getOption(KEY_LOG_LEVEL)));
    Logi("AIVision is initialized");
}

void AIVision::deinit()
{
    if (!s_initialized)
        return;

    s_initialized = false;
    s_config = nullptr;
    Logi("AIVision is deinitialized");
}

bool AIVision::isInitialized()
{
    return s_initialized;
}

std::string AIVision::getBasePath()
{
    std::string basePath = s_config->getOption(KEY_BASE_PATH);
    if (basePath.empty()) {
        basePath = EDGEAI_VISION_HOME;
    }
    Logi("BasePath: ", basePath);
    return basePath;
}

std::string AIVision::getModelFolderPath()
{
    return getBasePath() + "/model/";
}


}
