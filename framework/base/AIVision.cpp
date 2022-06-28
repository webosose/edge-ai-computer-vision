/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/log/Logger.h>

#ifdef USE_UPDATABLE_MODELS
#include <AIAdaptor.h>
#endif

#define KEY_LOG_LEVEL "LogLevel"
#define KEY_BASE_PATH "BasePath"

namespace aif {

bool AIVision::s_initialized = false;
std::string AIVision::s_basePath = "";
std::unique_ptr<ConfigReader> AIVision::s_config;

void AIVision::init(const std::string& basePath)
{
    if (s_initialized)
        return;

    s_config = std::make_unique<ConfigReader>(
            std::string(EDGEAI_VISION_HOME) + "/" + std::string(EDGEAI_VISION_CONFIG));
    Logger::init(Logger::strToLogLevel(s_config->getOption(KEY_LOG_LEVEL)));

    if (!basePath.empty()) {
        s_basePath = basePath;
    } else {
        s_basePath = s_config->getOption(KEY_BASE_PATH);
    }

    s_initialized = true;
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
    if (s_basePath.empty()) {
        s_basePath = EDGEAI_VISION_HOME;
    }
    return s_basePath;
}

#ifdef USE_UPDATABLE_MODELS
int AIVision::getUpdatableModelIndex(const std::string& modelName)
{
    std::vector<std::string> updatableModels {
        "face_detection_short_range.tflite",
        "posenet_mobilenet_v1_075_353_481_quant_decoder.tflite",
        "selfie_segmentation.tflite"
    };

    for (int i = 0; i < updatableModels.size(); i++) {
        if (updatableModels[i] == modelName) {
            return i;
        }
    }
    return -1;
}
#endif

std::string AIVision::getModelPath(const std::string& modelName)
{
#ifdef USE_UPDATABLE_MODELS
    AIAdaptor adapter;
    auto paths = adapter.getModelPaths("aif");

    int index = getUpdatableModelIndex(modelName);
    if (0 <= index && index < paths.size()) {
        return paths[index];
    }
#endif
    return getBasePath() + "/model/" + modelName;
}


}
