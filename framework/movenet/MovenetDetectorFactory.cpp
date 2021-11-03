#include <aif/movenet/MovenetDetectorFactory.h>
#include <aif/movenet/CpuMovenetDetector.h>

#ifdef USE_EDGETPU
#include <aif/movenet/EdgeTpuMovenetDetector.h>
#endif

namespace aif {

//------------------------------------------------------
// MovenetDetectorFactory
//------------------------------------------------------

/*static */
std::shared_ptr<MovenetDetector> MovenetDetectorFactory::create(const std::string& name)
{
    if (name == "movenet_cpu") {
        return std::make_shared<CpuMovenetDetector>();
    }

#ifdef USE_EDGETPU
    if (name == "movenet_edgetpu") {
        return std::make_shared<EdgeTpuMovenetDetector>();
    }
#endif

    return std::make_shared<CpuMovenetDetector>();

}

} // end of namespace aif
