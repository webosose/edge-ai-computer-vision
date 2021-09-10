#ifndef AIF_FACE_DETECTOR_H
#define AIF_FACE_DETECTOR_H

#include <aif/base/Detector.h>
#include <aif/face/FaceDescriptor.h>
#include <aif/face/FaceParam.h>

namespace aif {

class FaceDetector : public Detector
{
public:
    FaceDetector(
        const std::string& modelPath,
        const std::shared_ptr<DetectorParam>& param);

    virtual ~FaceDetector();

protected:
    t_aif_status preProcessing() override;
    t_aif_status postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor) override;

    t_aif_status generateAnchors(int width, int height);
    t_aif_status faceDetect(std::shared_ptr<Descriptor>& descriptor);
    float calculateScale(float min_scale, float max_scale, int stride_index, int num_strides);

protected:
    std::vector<t_aif_anchor> m_anchors;
};

} // end of namespace aif

#endif // AIF_FACE_DETECTOR_H
