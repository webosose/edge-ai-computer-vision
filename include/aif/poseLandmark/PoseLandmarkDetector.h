#ifndef AIF_POSELANDMARK_DETECTOR_H
#define AIF_POSELANDMARK_DETECTOR_H

#include <aif/base/Detector.h>
#include <aif/poseLandmark/PoseLandmarkDescriptor.h>
#include <aif/poseLandmark/PoseLandmarkParam.h>

namespace aif {

class PoseLandmarkDetector : public Detector
{
public:
    PoseLandmarkDetector(const std::string& modelPath);
    virtual ~PoseLandmarkDetector();

protected:
    std::shared_ptr<DetectorParam> createParam() override;
    t_aif_status fillInputTensor(const cv::Mat& img) override;
    t_aif_status preProcessing() override;
    t_aif_status postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor) override;

private:
    void printOutput();
};

} // end of namespace aif

#endif // AIF_POSELANDMARK_DETECTOR_H
