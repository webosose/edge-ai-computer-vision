#ifndef AIF_BODYPIX_DETECTOR_FACTORY_H
#define AIF_BODYPIX_DETECTOR_FACTORY_H

#include <aif/bodypix/BodypixDetector.h>

namespace aif {

//------------------------------------------------------
// BodypixDetectorFactory
//------------------------------------------------------

class BodypixDetectorFactory
{
public:
    static std::shared_ptr<BodypixDetector> create(const std::string& name);
};

} // end of namespace aif

#endif // AIF_BODYPIX_DETECTOR_FACTORY_H
