/*
 * Copyright (c) 2022-2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_EDGEAI_VISION_H
#define AIF_EDGEAI_VISION_H

#include <opencv2/opencv.hpp>
#include <mutex>
#include <map>

namespace aif {

class Pipe;
class ExtraOutput;
using ExtraOutputs = std::map<std::string, ExtraOutput>;

class EdgeAISolution {
  public:

    enum class SolutionPriority : unsigned int {
        PRIORITY_NONE = 0x0,   // not defined.
        MIN_LATENCY = 0x1,    // NPU
        MAX_PRECISION = 0x2,  // CPU or GPU

        MID_MODEL = 0x10, // mid
        LOW_MODEL = 0x20, // low

        MIN_LATENCY_MID_MODEL = (MIN_LATENCY | MID_MODEL), // NPU mid = 0x11
        MIN_LATENCY_LOW_MODEL = (MIN_LATENCY | LOW_MODEL), // NPU low = 0x21

        MAX_PRECISION_MID_MODEL = (MAX_PRECISION | MID_MODEL), // GPU mid = 0x12
        MAX_PRECISION_LOW_MODEL = (MAX_PRECISION | LOW_MODEL), // GPU low = 0x22
    };


    struct SolutionConfig {
        std::vector<int> roi_region;  ///< roi_x, roi_y, roi_w, roi_h
        int targetFps;                ///< minimum requested fps
        SolutionPriority priority;
        int numMaxPerson;
    };

    EdgeAISolution()
        : m_solutionType("") {}
    EdgeAISolution(const std::string &name)
        : m_solutionType(name) {}
    virtual ~EdgeAISolution() {}
    virtual std::string makeSolutionConfig(SolutionConfig config) = 0;

  private:
    std::string m_solutionType;
};


/**
 *  API Facade class for Edge AI Computer Vision
 */
class EdgeAIVision {
 public:
    /// Detector type enum class
    enum class DetectorType {
        FACE = 0,           ///< Face Detector (Default Model: face short range)
        POSE,               ///< Pose Detector (Default Model: posenet)
        SEGMENTATION,       ///< Segmentation Detector (Default Model: mediapipe
                            ///< selfie)
        TEXT,               ///< Text Detector (Default Model: paddleocr)
        SIGNLANGUAGEAREA,   ///< Sign Language Area Detector (Default Model:
                            ///< yolov5n custom)
        CUSTOM = 999,       ///< Custom Detector Type
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
     * @brief detect the result from input image
     * @param type detector type
     * @param input input image data
     * @param output output result (json string)
     * @param extraOutput extra output result
     * @return return true if success to detect the result
     */
    bool detect(DetectorType type, const cv::Mat& input, std::string& output, ExtraOutput& extraOutput);

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
     * @brief detect the result from input image
     * @param id pipe identifier
     * @param input input image data
     * @param output output result (json string)
     * @param extraOutputs extra outputs result
     * @return return true if success to detect the result
     */
    bool pipeDetect(
            const std::string& id,
            const cv::Mat& input,
            std::string& output,
            const ExtraOutputs& extraOutputs);

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

    /**
     * @brief return capable solution list by string vector type.
     * @return return capable solution list by string vector type.
     */
    std::vector<std::string> getCapableSolutionList();

    /**
     * @brief return default SolutionConfig
     * @return return default SolutionConfig to be used in User.
     */
    EdgeAISolution::SolutionConfig getDefaultSolutionConfig();

    /**
     * @brief generate Default AIF Param json to be used in User.
     * @param solutionName Solution Name to be used in User.
     * @return json string of option to be used in createDetector() / PipeCreate() API.
     */
    std::string generateAIFParam(const std::string &solutionName);

    /**
     * @brief generate Default AIF Param json to be used in User.
     * @param solutionName Solution Name to be used in User.
     * @param outputPath output json path wher the result will be written.
     * @return json string of option to be used in createDetector() / PipeCreate() API.
     */
    std::string generateAIFParam(const std::string &solutionName, const std::string &outputPath);

    /**
     * @brief generate Default AIF Param json to be used in User.
     * @param solutionName Solution Name to be used in User.
     * @param config Solution Config including roi region, fps, ...
     * @param outputPath output json path wher the result will be written.
       @return json string of option to be used in createDetector() / PipeCreate() API.
     */
    std::string generateAIFParam(const std::string &solutionName,
                                 EdgeAISolution::SolutionConfig config, const std::string &outputPath = "");

  private:
    static std::once_flag s_onceFlag;
    static std::unique_ptr<EdgeAIVision> s_instance;
    mutable std::mutex m_mutex;
    std::map<DetectorType, std::string> m_selectedModels;
    std::map<std::string, std::shared_ptr<Pipe>> m_pipeMap;

    EdgeAIVision() = default;
    std::string getDefaultModel(DetectorType type) const;
};

/// Extra output type enum class
enum class ExtraOutputType {
    UNKNOWN = 0,
    FLOAT_ARRAY = 1,
    UINT8_ARRAY = 2,
    INT8_ARRAY = 3,
};

/// ExtraOutput class
class ExtraOutput {
    public:
        ExtraOutput()
            : m_type(ExtraOutputType::UNKNOWN)
              , m_buffer(nullptr)
              , m_bytes(0) {}
        ExtraOutput(ExtraOutputType type, void* buffer, uint32_t bytes)
            : m_type(type)
              , m_buffer(buffer)
              , m_bytes(bytes) {}

        ExtraOutputType type() const { return m_type; }
        void* buffer() const { return m_buffer; }
        size_t bytes() const { return m_bytes; }

    private:
        ExtraOutputType m_type;
        void* m_buffer;
        size_t m_bytes;
};


} // end of namespace aif


/**@example face/main.cpp
/**@example pipe-sample/main.cpp
 * Simple example of how to use the Edge AI Vision apis.
 */

#endif  // AIF_EDGEAI_VISION
