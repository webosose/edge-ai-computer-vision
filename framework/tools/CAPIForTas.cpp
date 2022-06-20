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

const char* getDetectResult(const char* image, int type)
{
    cv::Mat input = cv::imread(image, cv::IMREAD_COLOR);

    EdgeAIVision::DetectorType detectorType = (EdgeAIVision::DetectorType)type;

    EdgeAIVision ai = EdgeAIVision::getInstance();
    ai.startup();

    ai.createDetector(detectorType);

    ai.detect(detectorType, input, output);

    ai.deleteDetector(detectorType);
    ai.shutdown();

    return output.c_str();
}

#ifdef __cplusplus
}
#endif
