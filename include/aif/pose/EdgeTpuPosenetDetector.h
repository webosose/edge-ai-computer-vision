#ifndef AIF_EDGETPU_POSENET_DETECTOR_H
#define AIF_EDGETPU_POSENET_DETECTOR_H

#include <aif/pose/PosenetDetector.h>
#include <edgetpu.h>

namespace aif {

class EdgeTpuPosenetDetector : public PosenetDetector
{
public:
    EdgeTpuPosenetDetector();
    virtual ~EdgeTpuPosenetDetector();

protected:
    t_aif_status compileModel() override;

protected:
    std::shared_ptr<edgetpu::EdgeTpuContext> m_edgetpuContext;
};

} // end of namespace aif

#endif // AIF_EDGETPU_POSENET_DETECTOR_H
