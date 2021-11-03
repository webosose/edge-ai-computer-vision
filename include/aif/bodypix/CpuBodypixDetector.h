#ifndef AIF_CPU_BODYPIX_DETECTOR_H
#define AIF_CPU_BODYPIX_DETECTOR_H

#include <aif/bodypix/BodypixDetector.h>

namespace aif {

class CpuBodypixDetector : public BodypixDetector
{
public:
    CpuBodypixDetector();
    virtual ~CpuBodypixDetector();

protected:
    t_aif_status compileModel() override;

};

} // end of namespace aif

#endif // AIF_CPU_BODYPIX_DETECTOR_H
