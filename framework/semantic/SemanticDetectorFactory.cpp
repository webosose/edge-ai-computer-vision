#include <aif/semantic/SemanticDetectorFactory.h>
#include <aif/semantic/CpuSemanticDetector.h>

#ifdef USE_EDGETPU
#include <aif/semantic/EdgeTpuSemanticDetector.h>
#endif

namespace aif {

//------------------------------------------------------
// SemanticDetectorFactory
//------------------------------------------------------

/*static */
std::shared_ptr<SemanticDetector> SemanticDetectorFactory::create(const std::string& name)
{
    if (name == "semantic_cpu") {
        return std::make_shared<CpuSemanticDetector>();
    }

#ifdef USE_EDGETPU
    if (name == "semantic_edgetpu") {
        return std::make_shared<EdgeTpuSemanticDetector>();
    }
#endif

    return std::make_shared<CpuSemanticDetector>();

}

} // end of namespace aif
