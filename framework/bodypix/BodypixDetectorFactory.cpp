#include <aif/bodypix/BodypixDetectorFactory.h>
#include <aif/bodypix/CpuBodypixDetector.h>

#ifdef USE_EDGETPU
#include <aif/bodypix/EdgeTpuBodypixDetector.h>
#endif

namespace aif {

//------------------------------------------------------
// BodypixDetectorFactory
//------------------------------------------------------

/*static */
std::shared_ptr<BodypixDetector> BodypixDetectorFactory::create(const std::string& name)
{
    if (name == "bodypix_cpu") {
        return std::make_shared<CpuBodypixDetector>();
    }

#ifdef USE_EDGETPU
    if (name == "bodypix_edgetpu") {
        return std::make_shared<EdgeTpuBodypixDetector>();
    }
#endif

    return std::make_shared<CpuBodypixDetector>();

}

} // end of namespace aif
