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
    static std::shared_ptr<PosenetDetector> create(
           const std::string& name,
           const std::string& model_path = "/usr/share/aif/model/posenet_mobilenet_v1_075_353_481_quant_decoder.tflite",
           const std::string& options = "");
};

} // end of namespace aif

#endif // AIF_POSENET_DETECTOR_FACTORY_H
