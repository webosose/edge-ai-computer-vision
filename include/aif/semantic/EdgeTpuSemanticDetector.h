#ifndef AIF_EDGETPU_SEMANTIC_DETECTOR_H
#define AIF_EDGETPU_SEMANTIC_DETECTOR_H

#include <aif/semantic/SemanticDetector.h>
#include <aif/semantic/SemanticDescriptor.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>

#include <edgetpu.h>

namespace aif {

class EdgeTpuSemanticDetector : public SemanticDetector
{
private:
    EdgeTpuSemanticDetector();

public:
    template<typename T1, typename T2> friend class DetectorFactoryRegistration;
    virtual ~EdgeTpuSemanticDetector();

protected:
    t_aif_status compileModel() override;

protected:
    std::shared_ptr<edgetpu::EdgeTpuContext> m_edgetpuContext;
};

DetectorFactoryRegistration<EdgeTpuSemanticDetector, SemanticDescriptor>
semantic_deeplabv3_edgetpu("semantic_deeplabv3_edgetpu");

} // end of namespace aif

#endif // AIF_EDGETPU_SEMANTIC_DETECTOR_H
