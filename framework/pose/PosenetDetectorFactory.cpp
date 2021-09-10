#include <aif/pose/PosenetDetectorFactory.h>
#include <aif/pose/CpuPosenetDetector.h>

#ifdef USE_EDGETPU
#include <aif/pose/EdgeTpuPosenetDetector.h>
#endif

namespace aif {

//------------------------------------------------------
// PosenetDetectorFactory
//------------------------------------------------------

/*static */
std::shared_ptr<PosenetDetector> PosenetDetectorFactory::create(const std::string& name)
{
    if (name == "posenet_cpu") {
        return std::make_shared<CpuPosenetDetector>();
    }

#ifdef USE_EDGETPU
    if (name == "posenet_edgetpu") {
        return std::make_shared<EdgeTpuPosenetDetector>();
    }
#endif

    return std::make_shared<CpuPosenetDetector>();

}

} // end of namespace aif
