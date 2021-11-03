#ifndef AIF_EDGETPU_SEMANTIC_DETECTOR_H
#define AIF_EDGETPU_SEMANTIC_DETECTOR_H

#include <aif/semantic/SemanticDetector.h>
#include <edgetpu.h>

namespace aif {

class EdgeTpuSemanticDetector : public SemanticDetector
{
public:
    EdgeTpuSemanticDetector();
    virtual ~EdgeTpuSemanticDetector();

protected:
    t_aif_status compileModel() override;

protected:
    std::shared_ptr<edgetpu::EdgeTpuContext> m_edgetpuContext;
};

} // end of namespace aif

#endif // AIF_EDGETPU_SEMANTIC_DETECTOR_H
