#ifndef AIF_CPU_SEMANTIC_DETECTOR_H
#define AIF_CPU_SEMANTIC_DETECTOR_H

#include <aif/semantic/SemanticDetector.h>
#include <aif/semantic/SemanticDescriptor.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>

namespace aif {

class CpuSemanticDetector : public SemanticDetector
{
public:
    CpuSemanticDetector();
    virtual ~CpuSemanticDetector();

protected:
    t_aif_status compileModel() override;

};

DetectorFactoryRegistration<CpuSemanticDetector, SemanticDescriptor>
semantic_deeplabv3_cpu("semantic_deeplabv3_cpu");
} // end of namespace aif

#endif // AIF_CPU_SEMANTIC_DETECTOR_H
