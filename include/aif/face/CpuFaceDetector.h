#ifndef AIF_CPU_FACE_DETECTOR_H
#define AIF_CPU_FACE_DETECTOR_H

#include <aif/face/FaceDetector.h>

namespace aif {

//------------------------------------------------------
// CpuFaceDetector
//------------------------------------------------------
class CpuFaceDetector : public FaceDetector
{
public:
    CpuFaceDetector(
        const std::string& modelPath,
        const std::shared_ptr<DetectorParam>& param);

    virtual ~CpuFaceDetector();

protected:

    t_aif_status compileModel() override;

    t_aif_status fillInputTensor(const cv::Mat& img) override;

    // virtual t_aif_status generateAnchors(int width, int height);

    // virtual t_aif_status faceDetect(FaceDescriptorList& foundFaces);
};

//------------------------------------------------------
// ShortRangeFaceDetector
//------------------------------------------------------
class ShortRangeFaceDetector : public CpuFaceDetector
{
public:
    ShortRangeFaceDetector();

    virtual ~ShortRangeFaceDetector();
};

//------------------------------------------------------
// FullRangeFaceDetector
//------------------------------------------------------
class FullRangeFaceDetector : public CpuFaceDetector
{
public:
    FullRangeFaceDetector();

    virtual ~FullRangeFaceDetector();
};

} // end of namespace aif

#endif // AIF_CPU_FACE_DETECTOR_H
