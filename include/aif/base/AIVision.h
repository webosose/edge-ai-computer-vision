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

#ifndef EDGEAI_VISION_CONFIG
#define EDGEAI_VISION_CONFIG "edgeai.config"
#endif

#define KEY_LOG_LEVEL "LogLevel"
#define KEY_BASE_PATH "BasePath"
#define KEY_REPORT_TYPE "ReportType"

namespace aif {

class AIVision {
public:
    static void init(const std::string& basePath = "");
    static void deinit();
    static bool isInitialized();
    static std::string getBasePath();
    static std::string getModelPath(const std::string& modelName);
#ifdef USE_UPDATABLE_MODELS
    static int getUpdatableModelIndex(const std::string& modelName);
#endif

private:
    static bool s_initialized;
    static std::string s_basePath;
    static std::unique_ptr<ConfigReader> s_config;
};

} // end of namespace aif

#endif
