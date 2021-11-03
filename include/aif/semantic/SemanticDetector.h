#ifndef AIF_SEMANTIC_DETECTOR_H
#define AIF_SEMANTIC_DETECTOR_H

#include <aif/base/Detector.h>
#include <aif/semantic/SemanticDescriptor.h>
#include <aif/semantic/SemanticParam.h>

namespace aif {

class SemanticDetector : public Detector
{
public:
    SemanticDetector(
            const std::string& modelPath,
            const std::shared_ptr<DetectorParam>& param);
    virtual ~SemanticDetector();

protected:
    t_aif_status fillInputTensor(const cv::Mat& img) override;
    t_aif_status preProcessing() override;
    t_aif_status postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor) override;

private:
    void printOutput();
};

} // end of namespace aif

#endif // AIF_SEMANTIC_DETECTOR_H
