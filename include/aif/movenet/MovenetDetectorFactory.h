#ifndef AIF_MOVENET_DETECTOR_FACTORY_H
#define AIF_MOVENET_DETECTOR_FACTORY_H

#include <aif/movenet/MovenetDetector.h>

namespace aif {

//------------------------------------------------------
// MovenetDetectorFactory
//------------------------------------------------------

class MovenetDetectorFactory
{
public:
    static std::shared_ptr<MovenetDetector> create(const std::string& name);
};

} // end of namespace aif

#endif // AIF_MOVENET_DETECTOR_FACTORY_H
