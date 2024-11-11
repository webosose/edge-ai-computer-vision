/*
 * Copyright (c) 2022-2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DelegateFactory.h>
#include <aif/base/ExtensionLoader.h>
#include <aif/log/Logger.h>
#include <aif/tools/PerformanceReporter.h>

#ifdef USE_UPDATABLE_MODELS
#include <AIAdaptor.h>
#endif

namespace aif {

bool AIVision::s_initialized = false;
std::string AIVision::s_basePath = "";
std::unique_ptr<ConfigReader> AIVision::s_config;
std::unique_ptr<ConfigReader> AIVision::s_override_config;

bool AIVision::init(const std::string& basePath)
{
    if (s_initialized)
        return true;

    s_config = std::make_unique<ConfigReader>(
            std::string(EDGEAI_VISION_HOME) + "/" + std::string(EDGEAI_VISION_CONFIG));
    s_override_config = std::make_unique<ConfigReader>(
            std::string(EDGEAI_VISION_HOME_SUB) + "/" + std::string(EDGEAI_VISION_CONFIG));

    mergeConfig(s_config, s_override_config);
    Logger::init(Logger::strToLogLevel(s_config->getOption(KEY_LOG_LEVEL)));

    std::vector<std::string> reportTypes = s_config->getOptionArray(KEY_REPORT_TYPE);
    if (reportTypes.size() > 0) {
        PerformanceReporter::get().clear();
        for (auto& type : reportTypes) {
            PerformanceReporter::get().addReportType(PerformanceReporter::strToReportType(type));
            Logi("ReportType: ", type);
        }
    }

    if (!basePath.empty()) {
        s_basePath = basePath;
    } else {
        s_basePath = s_config->getOption(KEY_BASE_PATH);
    }

    //ExtensionLoader initialization
    {
        std::string extensionPath = s_config->getOption(KEY_EXTENSION_PATH);
        std::vector<std::string> allowedExtensions = s_config->getOptionArray(KEY_ALLOWED_EXTENSIONS);
        std::string retryCount = s_config->getOption(KEY_EXTENSION_LOADER_RETRY_COUNT);
        std::string faultTolerance = s_config->getOption(KEY_EXTENSION_LOADER_FAULT_TOLEARANCE);
        std::string forceRegenRegistry = s_config->getOption(KEY_FORCE_REGEN_REGISTRY);

        if (extensionPath.empty())
            extensionPath = EDGEAI_VISION_EXTENSION_PATH;
        if (allowedExtensions.size() == 0) 
            allowedExtensions = {};
        if (!retryCount.empty())
            ExtensionLoader::get().setRetryCount(std::stoi(retryCount));
        if (!faultTolerance.empty()) {
            Logd("FaultTolerance: ", faultTolerance); //TODO: Check why "1" comes, not "true"
            ExtensionLoader::get().setFaultTolerance(faultTolerance == "1");
        }
        if (!forceRegenRegistry.empty()) {
            Logd("ForceRegenRegistry: ", forceRegenRegistry); //TODO: Check why "1" comes, not "true"
            ExtensionLoader::get().setForceRegenRegistry(forceRegenRegistry == "1");
        }

        if (ExtensionLoader::get().initRetry(true, extensionPath, allowedExtensions) != kAifOk) {
            Loge("Failed to initialize ExtensionLoader");
            return false;
        }
    }

    s_initialized = true;
    Logi("AIVision is initialized");
    return true;
}

void AIVision::deinit()
{
    if (!s_initialized)
        return;

    DetectorFactory::get().clear();
    DelegateFactory::get().clear();

    ExtensionLoader::get().unloadAllExtension();
    ExtensionLoader::get().clear();

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
        "yunet_yunet_final_360_640_simplify_float32.tflite"
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
std::string AIVision::getExtraModelPath(const std::string& modelName)
{
    return getBasePath() + "/extra_models/" + modelName;
}

void AIVision::mergeConfig(std::unique_ptr<ConfigReader>& target, std::unique_ptr<ConfigReader>& source)
{
    if (!target || !source || source->m_document.IsObject() == false) {
        return;
    }
    for (auto& member : source->m_document.GetObject()) {
        // If the member exists in the target, replace with source value
        if (target->m_document.HasMember(member.name)) {
            rapidjson::Value value(member.value, target->m_document.GetAllocator());
            target->m_document[member.name] = std::move(value);
            continue;
        }
        // If the member doesn't exist in the target, add it
        rapidjson::Value name(member.name, target->m_document.GetAllocator());
        rapidjson::Value value(member.value, target->m_document.GetAllocator());
        target->m_document.AddMember(name, value, target->m_document.GetAllocator());
    }
}

} // end of namespace aif

