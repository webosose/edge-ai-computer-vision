#ifndef AIF_CPU_MOVENET_DETECTOR_H
#define AIF_CPU_MOVENET_DETECTOR_H

#include <aif/movenet/MovenetDetector.h>
#include <aif/movenet/MovenetDescriptor.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>

namespace aif {

class CpuMovenetDetector : public MovenetDetector
{
public:
    CpuMovenetDetector();
    virtual ~CpuMovenetDetector();

protected:
    t_aif_status compileModel() override;

};

DetectorFactoryRegistration<CpuMovenetDetector, MovenetDescriptor>
movenet_thunder_cpu("movenet_thunder_cpu");

} // end of namespace aif

#endif // AIF_CPU_MOVENET_DETECTOR_H
