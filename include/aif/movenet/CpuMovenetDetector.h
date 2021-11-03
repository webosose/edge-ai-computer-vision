#ifndef AIF_CPU_MOVENET_DETECTOR_H
#define AIF_CPU_MOVENET_DETECTOR_H

#include <aif/movenet/MovenetDetector.h>

namespace aif {

class CpuMovenetDetector : public MovenetDetector
{
public:
    CpuMovenetDetector();
    virtual ~CpuMovenetDetector();

protected:
    t_aif_status compileModel() override;

};

} // end of namespace aif

#endif // AIF_CPU_MOVENET_DETECTOR_H
