#ifndef AIF_EDGETPU_BODYPIX_DETECTOR_H
#define AIF_EDGETPU_BODYPIX_DETECTOR_H

#include <aif/bodypix/BodypixDetector.h>
#include <aif/bodypix/BodypixDescriptor.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>

#include <edgetpu.h>

namespace aif {

class EdgeTpuBodypixDetector : public BodypixDetector
{
private:
    EdgeTpuBodypixDetector();

public:    
    template<typename T1, typename T2> friend class DetectorFactoryRegistration;
    virtual ~EdgeTpuBodypixDetector();

protected:
    t_aif_status compileModel() override;

protected:
    std::shared_ptr<edgetpu::EdgeTpuContext> m_edgetpuContext;
};

DetectorFactoryRegistration<EdgeTpuBodypixDetector, BodypixDescriptor>
bodypix_mobilenet_edgetpu("bodypix_mobilenet_edgetpu");

} // end of namespace aif

#endif // AIF_EDGETPU_BODYPIX_DETECTOR_H
