#ifndef AIF_EDGETPU_MOVENET_DETECTOR_H
#define AIF_EDGETPU_MOVENET_DETECTOR_H

#include <aif/movenet/MovenetDetector.h>
#include <edgetpu.h>

namespace aif {

class EdgeTpuMovenetDetector : public MovenetDetector
{
public:
    EdgeTpuMovenetDetector();
    virtual ~EdgeTpuMovenetDetector();

protected:
    t_aif_status compileModel() override;

protected:
    std::shared_ptr<edgetpu::EdgeTpuContext> m_edgetpuContext;
};

} // end of namespace aif

#endif // AIF_EDGETPU_MOVENET_DETECTOR_H
