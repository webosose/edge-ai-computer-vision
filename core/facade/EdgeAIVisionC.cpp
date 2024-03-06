/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/facade/EdgeAIVisionC.h>

using namespace aif;

bool edgeai_isStarted()
{
    return EdgeAIVision::getInstance().isStarted();
}

bool edgeai_startup(const std::string& basePath)
{
    return EdgeAIVision::getInstance().startup(basePath);
}

bool edgeai_shutdown()
{
    return EdgeAIVision::getInstance().shutdown();
}

bool edgeai_createDetector(int type, const std::string& option)
{
    return EdgeAIVision::getInstance().createDetector((EdgeAIVision::DetectorType)type, option);
}

bool edgeai_deleteDetector(int type)
{
    return EdgeAIVision::getInstance().deleteDetector((EdgeAIVision::DetectorType)type);
}

bool edgeai_detect(int type, const cv::Mat &input, std::string &output)
{
    return EdgeAIVision::getInstance().detect((EdgeAIVision::DetectorType)type, input, output);
}

bool edgeai_detectFromFile(int type, const std::string &inputPath, std::string &output)
{
    return EdgeAIVision::getInstance().detectFromFile((EdgeAIVision::DetectorType)type, inputPath, output);
}

bool edgeai_detectFromBase64(int type, const std::string &input, std::string &output)
{
    return EdgeAIVision::getInstance().detectFromBase64((EdgeAIVision::DetectorType)type, input, output);
}
