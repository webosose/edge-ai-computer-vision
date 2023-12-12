/*
 * Copyright (c) 2022-2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_EDGEAI_VISION_C_H
#define AIF_EDGEAI_VISION_C_H

#include <aif/facade/EdgeAIVision.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * C APIs for Edge AI Computer Vision
 */

/// Detector Type enum
enum DetectorType {
    FACE = 0,           ///< Face Detector (Default Model: face short range)
    POSE,               ///< Pose Detector (Default Model: posenet)
    SEGMENTATION,       ///< Segmentation Detector (Default Model: mediapipe
                        ///< selfie)
    TEXT,               ///< Text Detector (Default Model: paddleocr)
    SIGNLANGUAGEAREA,   ///< Sign Language Area Detector (Default Model:
                        ///< yolov5n custom)
};

/**
 * @brief check if Edge AI Vision is started or not.
 * @return return true if the Edge AI Vision is already started
 */
bool edgeai_isStarted();

/**
 * @brief startup Edge AI Vision
 * @param basePath base folder path for Edge AI Vision
 * @return return true if success to startup
 */
bool edgeai_startup(const std::string& basePath = "");

/**
 * @brief shutdown Edge AI Vision
 * @return return true if success to shutdown
 */
bool edgeai_shutdown();

/**
 * @brief create detector with specific DetectorType
 * @param type detector type
 * @param option detector option (setup specific model, param, delegates)
 * @return return true if success to create detector
 */
bool edgeai_createDetector(int type, const std::string& option);

/**
 * @brief delete detector with specific DetectorType
 * @param type detector type
 * @return return true if success to delete detector
 */
bool edgeai_deleteDetector(int type);

/**
 * @brief detect the result from input image
 * @param type detector type
 * @param input input image data
 * @param output output result (json string)
 * @return return true if success to detect the result
 */
bool edgeai_detect(int type, const cv::Mat &input, std::string &output);

/**
 * @brief detect the result from input image file
 * @param type detector type
 * @param inputPath input image file path
 * @param output output result (json string)
 * @return return true if success to detect the result from input image file
 */
bool edgeai_detectFromFile(int type, const std::string &inputPath, std::string &output);

/**
 * @brief detect the result from base64 input image
 * @param type detector type
 * @param input base64 input image
 * @param output output result (json string)
 * @return return true if success to detect the result from input image file
 */
bool edgeai_detectFromBase64(int type, const std::string &input, std::string &output);

#ifdef __cplusplus
}
#endif

#endif  // AIF_EDGEAI_VISION_C_H
