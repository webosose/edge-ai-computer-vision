#ifndef AIF_EDGETPU_MNAME_DETECTOR_H
#define AIF_EDGETPU_MNAME_DETECTOR_H

#include <aif/mname/MNameDetector.h>
#include <aif/mname/MNameDescriptor.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <edgetpu.h>

namespace aif {

class EdgeTpuMNameDetector : public MNameDetector
{
private:
    EdgeTpuMNameDetector();

public:
    template<typename T1, typename T2> friend class DetectorFactoryRegistration;
    virtual ~EdgeTpuMNameDetector();

protected:
    t_aif_status compileModel() override;

protected:
    std::shared_ptr<edgetpu::EdgeTpuContext> m_edgetpuContext;
};

DetectorFactoryRegistration<EdgeTpuMNameDetector, MNameDescriptor>
mname_model_edgetpu("mname_model_edgetpu");

} // end of namespace aif

#endif // AIF_EDGETPU_MNAME_DETECTOR_H
