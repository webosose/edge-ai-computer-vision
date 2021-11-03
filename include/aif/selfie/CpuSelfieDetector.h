#ifndef AIF_CPU_SELFIE_DETECTOR_H
#define AIF_CPU_SELFIE_DETECTOR_H

#include <aif/selfie/SelfieDetector.h>

namespace aif {

class CpuSelfieDetector : public SelfieDetector
{
public:
    CpuSelfieDetector();
    virtual ~CpuSelfieDetector();

protected:
    t_aif_status compileModel() override;

};

} // end of namespace aif

#endif // AIF_CPU_SELFIE_DETECTOR_H
