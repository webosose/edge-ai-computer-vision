#ifndef AIF_FACE_DETECTOR_FACTORY_H
#define AIF_FACE_DETECTOR_FACTORY_H

#include <aif/face/FaceDetector.h>

namespace aif {

//------------------------------------------------------
// FaceDetectorFactory
//------------------------------------------------------

class FaceDetectorFactory
{
public:
    static std::shared_ptr<FaceDetector> create(const std::string& name);
};

} // end of namespace aif

#endif // AIF_FACE_DETECTOR_FACTORY_H