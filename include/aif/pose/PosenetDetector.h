#ifndef AIF_POSENET_DETECTOR_H
#define AIF_POSENET_DETECTOR_H

#include <aif/base/Detector.h>
#include <aif/pose/PosenetDescriptor.h>
#include <aif/pose/PosenetParam.h>

namespace aif {

class PosenetDetector : public Detector
{
public:
    PosenetDetector(
            const std::string& modelPath,
            const std::shared_ptr<DetectorParam>& param);
    virtual ~PosenetDetector();

protected:
    t_aif_status fillInputTensor(const cv::Mat& img) override;
    t_aif_status preProcessing() override;
    t_aif_status postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor) override;
};

} // end of namespace aif

#endif // AIF_POSENET_DETECTOR_H
