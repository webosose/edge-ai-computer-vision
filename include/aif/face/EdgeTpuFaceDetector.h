#ifndef AIF_EDGE_FACE_DETECTOR_H
#define AIF_EDGE_FACE_DETECTOR_H

#include <aif/face/FaceDetector.h>
#include <aif/face/FaceDescriptor.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <edgetpu.h>

namespace aif {

//------------------------------------------------------
// EdgeTpuFaceDetector
//------------------------------------------------------
class EdgeTpuFaceDetector : public FaceDetector
{
public:
    EdgeTpuFaceDetector(
        const std::string& modelPath,
        const std::shared_ptr<DetectorParam>& param);

    virtual ~EdgeTpuFaceDetector();

protected:

    t_aif_status compileModel() override;

    t_aif_status fillInputTensor(const cv::Mat& img) override;

protected:
    std::shared_ptr<edgetpu::EdgeTpuContext> m_edgetpuContext;
};

//------------------------------------------------------
// EdgeTpuShortRangeFaceDetector
//------------------------------------------------------
class EdgeTpuShortRangeFaceDetector : public EdgeTpuFaceDetector
{
public:
    EdgeTpuShortRangeFaceDetector();

    virtual ~EdgeTpuShortRangeFaceDetector();
};

DetectorFactoryRegistration<EdgeTpuShortRangeFaceDetector, FaceDescriptor>
face_detect_edgetpu("face_short_range_edgetpu");

} // end of namespace aif

#endif // AIF_EDGE_FACE_DETECTOR_H
