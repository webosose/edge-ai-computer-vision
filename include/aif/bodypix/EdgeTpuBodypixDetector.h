#ifndef AIF_EDGETPU_BODYPIX_DETECTOR_H
#define AIF_EDGETPU_BODYPIX_DETECTOR_H

#include <aif/bodypix/BodypixDetector.h>
#include <edgetpu.h>

namespace aif {

class EdgeTpuBodypixDetector : public BodypixDetector
{
public:
    EdgeTpuBodypixDetector();
    virtual ~EdgeTpuBodypixDetector();

protected:
    t_aif_status compileModel() override;

protected:
    std::shared_ptr<edgetpu::EdgeTpuContext> m_edgetpuContext;
};

} // end of namespace aif

#endif // AIF_EDGETPU_BODYPIX_DETECTOR_H
