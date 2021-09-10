#ifndef AIF_POSENET_DETECTOR_FACTORY_H
#define AIF_POSENET_DETECTOR_FACTORY_H

#include <aif/pose/PosenetDetector.h>

namespace aif {

//------------------------------------------------------
// PosenetDetectorFactory
//------------------------------------------------------

class PosenetDetectorFactory
{
public:
    static std::shared_ptr<PosenetDetector> create(const std::string& name);
};

} // end of namespace aif

#endif // AIF_POSENET_DETECTOR_FACTORY_H
