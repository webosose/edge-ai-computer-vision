#include <aif/mname/MNameDetectorFactory.h>
#include <aif/mname/CpuMNameDetector.h>

#ifdef USE_EDGETPU
#include <aif/mname/EdgeTpuMNameDetector.h>
#endif

namespace aif {

//------------------------------------------------------
// MNameDetectorFactory
//------------------------------------------------------

/*static */
std::shared_ptr<MNameDetector> MNameDetectorFactory::create(const std::string& name)
{
    if (name == "mname_cpu") {
        return std::make_shared<CpuMNameDetector>();
    }

#ifdef USE_EDGETPU
    if (name == "mname_edgetpu") {
        return std::make_shared<EdgeTpuMNameDetector>();
    }
#endif

    return std::make_shared<CpuMNameDetector>();

}

} // end of namespace aif
