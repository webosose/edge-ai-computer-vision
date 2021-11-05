#include <aif/pose/PosenetDetectorFactory.h>
#include <aif/pose/CpuPosenetDetector.h>
#include <aif/pose/ArmNNPosenetDetector.h>

#ifdef USE_EDGETPU
#include <aif/pose/EdgeTpuPosenetDetector.h>
#endif

namespace aif {

//------------------------------------------------------
// PosenetDetectorFactory
//------------------------------------------------------

/*static */
std::shared_ptr<PosenetDetector> PosenetDetectorFactory::create(
        const std::string& name,
        const std::string& model_path,
        const std::string& options)
{
    if (name == "posenet_cpu") {
        return std::make_shared<CpuPosenetDetector>();
    }

    if (name == "posenet_cpu_armnn") {
        return std::make_shared<ArmNNPosenetDetector>(model_path, options); 
    }

#ifdef USE_EDGETPU
    if (name == "posenet_edgetpu") {
        return std::make_shared<EdgeTpuPosenetDetector>();
    }
#endif

    return std::make_shared<CpuPosenetDetector>();

}

} // end of namespace aif
