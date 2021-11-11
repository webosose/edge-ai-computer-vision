#ifndef AIF_MNAME_DETECTOR_H
#define AIF_MNAME_DETECTOR_H

#include <aif/base/Detector.h>
#include <aif/mname/MNameDescriptor.h>
#include <aif/mname/MNameParam.h>

namespace aif {

class MNameDetector : public Detector
{
public:
    MNameDetector(
            const std::string& modelPath,
            const std::shared_ptr<DetectorParam>& param);
    virtual ~MNameDetector();

protected:
    t_aif_status fillInputTensor(const cv::Mat& img) override;
    t_aif_status preProcessing() override;
    t_aif_status postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor) override;

private:
    void printOutput();
};

} // end of namespace aif

#endif // AIF_MNAME_DETECTOR_H
