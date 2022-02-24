#ifndef AIF_EDGETPU_HANDLANDMARK_DETECTOR_H
#define AIF_EDGETPU_HANDLANDMARK_DETECTOR_H

#include <aif/handLandmark/HandLandmarkDetector.h>
#include <aif/handLandmark/HandLandmarkDescriptor.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <edgetpu.h>

namespace aif {

class EdgeTpuHandLandmarkDetector : public HandLandmarkDetector
{
private:
    EdgeTpuHandLandmarkDetector();

public:
    template<typename T1, typename T2> friend class DetectorFactoryRegistration;
    virtual ~EdgeTpuHandLandmarkDetector();

protected:
    t_aif_status compileModel() override;

protected:
    std::shared_ptr<edgetpu::EdgeTpuContext> m_edgetpuContext;
};

DetectorFactoryRegistration<EdgeTpuHandLandmarkDetector, HandLandmarkDescriptor>
handLandmark_model_edgetpu("handLandmark_model_edgetpu");

} // end of namespace aif

#endif // AIF_EDGETPU_HANDLANDMARK_DETECTOR_H
