#include <aif/selfie/SelfieDetectorFactory.h>
#include <aif/selfie/CpuSelfieDetector.h>

#ifdef USE_EDGETPU
//#include "EdgeTpuSelfieDetector.h"
#endif

namespace aif {

//------------------------------------------------------
// SelfieDetectorFactory
//------------------------------------------------------

/*static */
std::shared_ptr<SelfieDetector> SelfieDetectorFactory::create(const std::string& name)
{
    if (name == "selfie_cpu") {
        return std::make_shared<CpuSelfieDetector>();
    }

#ifdef USE_EDGETPU
//    if (name == "selfie_edgetpu") {
//        return std::make_shared<EdgeTpuSelfieDetector>();
//    }
#endif

    return std::make_shared<CpuSelfieDetector>();

}

} // end of namespace aif
