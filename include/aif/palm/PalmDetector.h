#ifndef AIF_PALM_DETECTOR_H
#define AIF_PALM_DETECTOR_H

#include <aif/base/Detector.h>
#include <aif/palm/PalmDescriptor.h>
#include <aif/palm/PalmParam.h>


namespace aif {

class PalmDetector : public Detector
{
public:
    PalmDetector(const std::string& modelPath);
    virtual ~PalmDetector();

protected:
    std::shared_ptr<DetectorParam> createParam() override;
    t_aif_status fillInputTensor(const cv::Mat& img) override;
    t_aif_status preProcessing() override;
    t_aif_status postProcessing(const cv::Mat& img,
            std::shared_ptr<Descriptor>& descriptor) override;

    t_aif_status generateAnchors(int width, int height);
    t_aif_status palmDetect(std::shared_ptr<Descriptor>& descriptor);
    float calculateScale(float min_scale, float max_scale, int stride_index, int num_strides);

private:
    void printOutput();

protected:
    std::vector<t_aif_anchor> m_anchors;
};

} // end of namespace aif

#endif // AIF_PALM_DETECTOR_H
