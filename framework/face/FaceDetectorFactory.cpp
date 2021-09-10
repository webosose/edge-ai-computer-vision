#include <aif/face/FaceDetectorFactory.h>
#include <aif/face/CpuFaceDetector.h>

#ifdef USE_EDGETPU
#include <aif/face/EdgeTpuFaceDetector.h>
#endif

namespace aif {

//------------------------------------------------------
// FaceDetectorFactory
//------------------------------------------------------

/*static */
std::shared_ptr<FaceDetector> FaceDetectorFactory::create(const std::string& name)
{
    if (name == "short_range") {
        return std::make_shared<ShortRangeFaceDetector>();
    } else if (name == "full_range") {
        return std::make_shared<FullRangeFaceDetector>();
    }

#ifdef USE_EDGETPU
    if (name == "short_range_edgetpu") {
        return std::make_shared<EdgeTpuShortRangeFaceDetector>();
    }

#endif

    return std::make_shared<ShortRangeFaceDetector>();

}

} // end of namespace aif