/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/facade/EdgeAIVision.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*
enum class DetectorType {
    FACE = 0,           ///< Face Detector (Default Model: face short range)
    POSE,               ///< Pose Detector (Default Model: posenet)
    SEGMENTATION,       ///< Segmentation Detector (Default Model: mediapipe selfie)
};
*/

using namespace aif;
static std::string output;


static const char* _detect(const char* image, int type, const char* option)
{
    cv::Mat input = cv::imread(image, cv::IMREAD_COLOR);

    EdgeAIVision::DetectorType detectorType = (EdgeAIVision::DetectorType)type;

    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ai.startup();

    ai.createDetector(detectorType, option);

    ai.detect(detectorType, input, output);

    ai.deleteDetector(detectorType);
    ai.shutdown();

    return output.c_str();
}

const char* getDetectResult(const char* image, int type)
{
    return _detect(image, type, "");
}

const char* getDetectResultWithOption(const char* image, int type, const char* option)
{
    return _detect(image, type, option);
}

#ifdef __cplusplus
}
#endif
