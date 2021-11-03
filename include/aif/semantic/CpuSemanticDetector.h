#ifndef AIF_CPU_SEMANTIC_DETECTOR_H
#define AIF_CPU_SEMANTIC_DETECTOR_H

#include <aif/semantic/SemanticDetector.h>

namespace aif {

class CpuSemanticDetector : public SemanticDetector
{
public:
    CpuSemanticDetector();
    virtual ~CpuSemanticDetector();

protected:
    t_aif_status compileModel() override;

};

} // end of namespace aif

#endif // AIF_CPU_SEMANTIC_DETECTOR_H
