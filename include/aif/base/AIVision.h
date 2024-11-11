/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_AIVISION_H
#define AIF_AIVISION_H

#include <aif/tools/ConfigReader.h>

#include <memory>

#ifndef EDGEAI_VISION_HOME
#define EDGEAI_VISION_HOME "/usr/share/aif"
#endif

#ifndef EDGEAI_VISION_HOME_SUB
#define EDGEAI_VISION_HOME_SUB "/home/root"
#endif

#ifndef EDGEAI_VISION_CONFIG
#define EDGEAI_VISION_CONFIG "edgeai.config"
#endif

#define KEY_LOG_LEVEL "LogLevel"
#define KEY_BASE_PATH "BasePath"
#define KEY_REPORT_TYPE "ReportType"
#define KEY_EXTENSION_PATH "ExtensionPath"
#define KEY_ALLOWED_EXTENSIONS "AllowedExtensions"
#define KEY_EXTENSION_LOADER_RETRY_COUNT "ExtensionLoaderRetryCount"
#define KEY_EXTENSION_LOADER_FAULT_TOLEARANCE "ExtensionLoaderFaultTolerance"
#define KEY_FORCE_REGEN_REGISTRY "ForceRegenRegistry"

namespace aif {

class AIVision {
public:
    static bool init(const std::string& basePath = "");
    static void deinit();
    static bool isInitialized();
    static std::string getBasePath();
    static std::string getModelPath(const std::string& modelName);
    static std::string getExtraModelPath(const std::string& modelName);
    static void mergeConfig(std::unique_ptr<ConfigReader>& target, std::unique_ptr<ConfigReader>& source);
#ifdef USE_UPDATABLE_MODELS
    static int getUpdatableModelIndex(const std::string& modelName);
#endif

private:
    static bool s_initialized;
    static std::string s_basePath;
    static std::unique_ptr<ConfigReader> s_config;
    static std::unique_ptr<ConfigReader> s_override_config;
};

} // end of namespace aif

#endif
