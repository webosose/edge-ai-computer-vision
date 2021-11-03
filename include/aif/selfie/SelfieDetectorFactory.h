#ifndef AIF_SELFIE_DETECTOR_FACTORY_H
#define AIF_SELFIE_DETECTOR_FACTORY_H

#include <aif/selfie/SelfieDetector.h>

namespace aif {

//------------------------------------------------------
// SelfieDetectorFactory
//------------------------------------------------------

class SelfieDetectorFactory
{
public:
    static std::shared_ptr<SelfieDetector> create(const std::string& name);
};

} // end of namespace aif

#endif // AIF_SELFIE_DETECTOR_FACTORY_H
