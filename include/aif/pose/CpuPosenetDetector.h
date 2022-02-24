#ifndef AIF_CPU_POSENET_DETECTOR_H
#define AIF_CPU_POSENET_DETECTOR_H

#include <aif/pose/PosenetDetector.h>
#include <aif/pose/PosenetDescriptor.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>

namespace aif {

class CpuPosenetDetector : public PosenetDetector
{
private:
    CpuPosenetDetector();

public:
    template<typename T1, typename T2> friend class DetectorFactoryRegistration;
    virtual ~CpuPosenetDetector();

protected:
    t_aif_status compileModel() override;

};

DetectorFactoryRegistration<CpuPosenetDetector, PosenetDescriptor>
posenet_mobilenet_cpu("posenet_mobilenet_cpu");

} // end of namespace aif

#endif // AIF_CPU_POSENET_DETECTOR_H
