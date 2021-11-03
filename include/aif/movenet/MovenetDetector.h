#ifndef AIF_MOVENET_DETECTOR_H
#define AIF_MOVENET_DETECTOR_H

#include <aif/base/Detector.h>
#include <aif/movenet/MovenetDescriptor.h>
#include <aif/movenet/MovenetParam.h>


namespace aif {

class MovenetDetector : public Detector
{
public:
    MovenetDetector(
            const std::string& modelPath,
            const std::shared_ptr<DetectorParam>& param);
    virtual ~MovenetDetector();

protected:
    t_aif_status fillInputTensor(const cv::Mat& img) override;
    t_aif_status preProcessing() override;
    t_aif_status postProcessing(const cv::Mat& img,
            std::shared_ptr<Descriptor>& descriptor) override;

private:
    void printOutput();
};

} // end of namespace aif

#endif // AIF_MOVENET_DETECTOR_H
