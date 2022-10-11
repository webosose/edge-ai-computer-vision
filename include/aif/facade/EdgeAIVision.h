/*
 * Copyright (c) 2022-2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_EDGEAI_VISION_H
#define AIF_EDGEAI_VISION_H

#include <opencv2/opencv.hpp>
#include <mutex>

namespace aif {

class Pipe;
/**
 *  API Facade class for Edge AI Computer Vision
 */

class EdgeAIVision {
 public:
    /// Detector Type enum class
    enum class DetectorType {
        FACE = 0,     ///< Face Detector (Default Model: face short range)
        POSE,         ///< Pose Detector (Default Model: posenet)
        SEGMENTATION, ///< Segmentation Detector (Default Model: mediapipe
                      ///< selfie)
        TEXT,         ///< Text Detector (Default Model: paddleocr)
    };

    EdgeAIVision(const EdgeAIVision& other) = delete;
    EdgeAIVision& operator=(const EdgeAIVision& rhs) = delete;

    /**
     * @brief get instance for Edge AI Vision
     */
    static EdgeAIVision& getInstance();

    /**
     * @brief check if Edge AI Vision is started or not.
     * @return return true if the Edge AI Vision is already started
     */
    bool isStarted() const;

    /**
     * @brief startup Edge AI Vision
     * @param basePath base folder path for Edge AI Vision
     * @return return true if success to startup
     */
    bool startup(const std::string& basePath = "");

    /**
     * @brief shutdown Edge AI Vision
     * @return return true if success to shutdown
     */
    bool shutdown();

    /**
     * @brief create detector with specific DetectorType
     * @param type detector type
     * @param option detector option (setup specific model, param, delegates)
     * @return return true if success to create detector
     */
    bool createDetector(DetectorType type, const std::string& option = "");

    /**
     * @brief delete detector with specific DetectorType
     * @param type detector type
     * @return return true if success to delete detector
     */
    bool deleteDetector(DetectorType type);

    /**
     * @brief detect the result from input image
     * @param type detector type
     * @param input input image data
     * @param output output result (json string)
     * @return return true if success to detect the result
     */
    bool detect(DetectorType type, const cv::Mat& input, std::string& output);

    /**
     * @brief detect the result from input image file
     * @param type detector type
     * @param inputPath input image file path
     * @param output output result (json string)
     * @return return true if success to detect the result from input image file
     */
    bool detectFromFile(DetectorType type, const std::string& inputPath,
                        std::string& output);

    /**
     * @brief detect the result from base64 input image
     * @param type detector type
     * @param input base64 input image
     * @param output output result (json string)
     * @return return true if success to detect the result from input image data
     */
    bool detectFromBase64(DetectorType type, const std::string& input,
                          std::string& output);

    /**
     * @brief create pipe with pipeline config
     * @param id pipe identifier 
     * @param option pipe configuration
     * @return return true if success to create pipe
     */
    bool pipeCreate(const std::string& id, const std::string& option);

    /**
     * @brief delete pipe with id
     * @param id pipe identifier
     * @return return true if success to delete pipe
     */
    bool pipeDelete(const std::string& id);

    /**
     * @brief detect the result from input image
     * @param id pipe identifier 
     * @param input input image data
     * @param output output result (json string)
     * @return return true if success to detect the result
     */
    bool pipeDetect(
            const std::string& id,
            const cv::Mat& input,
            std::string& output);

    /**
     * @brief detect the result from input image file
     * @param id pipe identifier 
     * @param inputPath input image file path
     * @param output output result (json string)
     * @return return true if success to detect the result from input image file
     */
    bool pipeDetectFromFile(
            const std::string& id,
            const std::string& inputPath,
            std::string& output);

    /**
     * @brief detect the result from base64 input image
     * @param id pipe identifier 
     * @param input base64 input image
     * @param output output result (json string)
     * @return return true if success to detect the result from input image data
     */
    bool pipeDetectFromBase64(
            const std::string& id,
            const std::string& input,
            std::string& output);

  private:
    static std::once_flag s_onceFlag;
    static std::unique_ptr<EdgeAIVision> s_instance;
    mutable std::mutex m_mutex;
    std::map<DetectorType, std::string> m_selectedModels;
    std::map<std::string, std::shared_ptr<Pipe>> m_pipeMap;

    EdgeAIVision() = default;
    std::string getDefaultModel(DetectorType type) const;
};

} // end of namespace aif


/**@example face/main.cpp
/**@example pipe-sample/main.cpp
/**@example fittv-sample/main.cpp
 * Simple example of how to use the Edge AI Vision apis.
 */

#endif  // AIF_EDGEAI_VISION
