#ifndef AIF_MNAME_DETECTOR_FACTORY_H
#define AIF_MNAME_DETECTOR_FACTORY_H

#include <aif/mname/MNameDetector.h>

namespace aif {

//------------------------------------------------------
// MNameDetectorFactory
//------------------------------------------------------

class MNameDetectorFactory
{
public:
    static std::shared_ptr<MNameDetector> create(const std::string& name);
};

} // end of namespace aif

#endif // AIF_MNAME_DETECTOR_FACTORY_H
