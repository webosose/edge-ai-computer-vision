#ifndef AIF_BODYPIX_DETECTOR_H
#define AIF_BODYPIX_DETECTOR_H

#include <aif/base/Detector.h>
#include <aif/bodypix/BodypixDescriptor.h>
#include <aif/bodypix/BodypixParam.h>

namespace aif {

class BodypixDetector : public Detector
{
public:
    BodypixDetector(
            const std::string& modelPath,
            const std::shared_ptr<DetectorParam>& param);
    virtual ~BodypixDetector();

protected:
    t_aif_status fillInputTensor(const cv::Mat& img) override;
    t_aif_status preProcessing() override;
    t_aif_status postProcessing(const cv::Mat& img,
            std::shared_ptr<Descriptor>& descriptor) override;
};

} // end of namespace aif

#endif // AIF_BODYPIX_DETECTOR_H
