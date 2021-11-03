#ifndef AIF_SEMANTIC_DETECTOR_FACTORY_H
#define AIF_SEMANTIC_DETECTOR_FACTORY_H

#include <aif/semantic/SemanticDetector.h>

namespace aif {

//------------------------------------------------------
// SemanticDetectorFactory
//------------------------------------------------------

class SemanticDetectorFactory
{
public:
    static std::shared_ptr<SemanticDetector> create(const std::string& name);
};

} // end of namespace aif

#endif // AIF_SEMANTIC_DETECTOR_FACTORY_H
