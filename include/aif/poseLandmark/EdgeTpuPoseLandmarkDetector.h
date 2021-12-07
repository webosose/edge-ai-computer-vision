#ifndef AIF_EDGETPU_POSELANDMARK_DETECTOR_H
#define AIF_EDGETPU_POSELANDMARK_DETECTOR_H

#include <aif/poseLandmark/PoseLandmarkDetector.h>
#include <aif/poseLandmark/PoseLandmarkDescriptor.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <edgetpu.h>

namespace aif {

class EdgeTpuPoseLandmarkDetector : public PoseLandmarkDetector
{
private:
    EdgeTpuPoseLandmarkDetector();

public:
    template<typename T1, typename T2> friend class DetectorFactoryRegistration;
    virtual ~EdgeTpuPoseLandmarkDetector();

protected:
    t_aif_status compileModel() override;

protected:
    std::shared_ptr<edgetpu::EdgeTpuContext> m_edgetpuContext;
};

DetectorFactoryRegistration<EdgeTpuPoseLandmarkDetector, PoseLandmarkDescriptor>
poseLandmark_model_edgetpu("poseLandmark_model_edgetpu");

} // end of namespace aif

#endif // AIF_EDGETPU_POSELANDMARK_DETECTOR_H
