#ifndef AIF_EDGETPU_POSENET_DETECTOR_H
#define AIF_EDGETPU_POSENET_DETECTOR_H

#include <aif/pose/PosenetDetector.h>
#include <aif/pose/PosenetDescriptor.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <edgetpu.h>

namespace aif {

class EdgeTpuPosenetDetector : public PosenetDetector
{
private:
    EdgeTpuPosenetDetector();

public:    
    template<typename T1, typename T2> friend class DetectorFactoryRegistration;
    virtual ~EdgeTpuPosenetDetector();

protected:
    t_aif_status compileModel() override;

protected:
    std::shared_ptr<edgetpu::EdgeTpuContext> m_edgetpuContext;
};

DetectorFactoryRegistration<EdgeTpuPosenetDetector, PosenetDescriptor>
posenet_mobilenet_edgetpu("posenet_mobilenet_edgetpu");

} // end of namespace aif

#endif // AIF_EDGETPU_POSENET_DETECTOR_H
