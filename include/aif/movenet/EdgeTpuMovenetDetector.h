#ifndef AIF_EDGETPU_MOVENET_DETECTOR_H
#define AIF_EDGETPU_MOVENET_DETECTOR_H

#include <aif/movenet/MovenetDetector.h>
#include <aif/movenet/MovenetDescriptor.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <edgetpu.h>

namespace aif {

class EdgeTpuMovenetDetector : public MovenetDetector
{
public:
    EdgeTpuMovenetDetector();
    virtual ~EdgeTpuMovenetDetector();

protected:
    t_aif_status compileModel() override;

protected:
    std::shared_ptr<edgetpu::EdgeTpuContext> m_edgetpuContext;
};

DetectorFactoryRegistration<EdgeTpuMovenetDetector, MovenetDescriptor>
movenet_thunder_edgetpu("movenet_thunder_edgetpu");

} // end of namespace aif

#endif // AIF_EDGETPU_MOVENET_DETECTOR_H
