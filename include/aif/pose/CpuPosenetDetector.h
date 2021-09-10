#ifndef AIF_CPU_POSENET_DETECTOR_H
#define AIF_CPU_POSENET_DETECTOR_H

#include <aif/pose/PosenetDetector.h>

namespace aif {

class CpuPosenetDetector : public PosenetDetector
{
public:
    CpuPosenetDetector();
    virtual ~CpuPosenetDetector();

protected:
    t_aif_status compileModel() override;

};

} // end of namespace aif

#endif // AIF_CPU_POSENET_DETECTOR_H
