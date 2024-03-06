#include <aifex/face/YuNetFaceDetectorV2s.h>
#include <aifex/face/YuNetFaceParam.h>
#include <aif/log/Logger.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>


namespace aif {

YuNetFaceDetectorV2_180_320::YuNetFaceDetectorV2_180_320()
: YuNetFaceDetectorV2("yunet_yunet_final_180_320_simplify_float32.tflite") {}

YuNetFaceDetectorV2_180_320_Quant::YuNetFaceDetectorV2_180_320_Quant()
: YuNetFaceDetectorV2("yunet_yunet_final_180_320_simplify_float16_quant.tflite") {}

YuNetFaceDetectorV2_240_320::YuNetFaceDetectorV2_240_320()
: YuNetFaceDetectorV2("yunet_yunet_final_240_320_simplify_float32.tflite") {}

YuNetFaceDetectorV2_240_320_Quant::YuNetFaceDetectorV2_240_320_Quant()
: YuNetFaceDetectorV2("yunet_yunet_final_240_320_simplify_float16_quant.tflite") {}

YuNetFaceDetectorV2_320_320::YuNetFaceDetectorV2_320_320()
: YuNetFaceDetectorV2("yunet_yunet_final_320_320_simplify_float32.tflite") {}

YuNetFaceDetectorV2_320_320_Quant::YuNetFaceDetectorV2_320_320_Quant()
: YuNetFaceDetectorV2("yunet_yunet_final_320_320_simplify_float16_quant.tflite") {}

YuNetFaceDetectorV2_360_640::YuNetFaceDetectorV2_360_640()
: YuNetFaceDetectorV2("yunet_yunet_final_360_640_simplify_float32.tflite") {}

YuNetFaceDetectorV2_360_640_Quant::YuNetFaceDetectorV2_360_640_Quant()
: YuNetFaceDetectorV2("yunet_yunet_final_360_640_simplify_float16_quant.tflite") {}

YuNetFaceDetectorV2_480_640::YuNetFaceDetectorV2_480_640()
: YuNetFaceDetectorV2("yunet_yunet_final_480_640_simplify_float32.tflite") {}

YuNetFaceDetectorV2_480_640_Quant::YuNetFaceDetectorV2_480_640_Quant()
: YuNetFaceDetectorV2("yunet_yunet_final_480_640_simplify_float16_quant.tflite") {}

YuNetFaceDetectorV2_640_640::YuNetFaceDetectorV2_640_640()
: YuNetFaceDetectorV2("yunet_yunet_final_640_640_simplify_float32.tflite") {}

YuNetFaceDetectorV2_640_640_Quant::YuNetFaceDetectorV2_640_640_Quant()
: YuNetFaceDetectorV2("yunet_yunet_final_640_640_simplify_float16_quant.tflite") {}

YuNetFaceDetectorV2_270_480::YuNetFaceDetectorV2_270_480()
: YuNetFaceDetectorV2("yunet_yunet_final_270_480_simplify_float32.tflite") {}

YuNetFaceDetectorV2_270_480_Quant::YuNetFaceDetectorV2_270_480_Quant()
: YuNetFaceDetectorV2("yunet_yunet_final_270_480_simplify_float16_quant.tflite") {}


} // end of namespace aif
