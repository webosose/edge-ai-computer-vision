#include <aif/face/CpuFaceDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/tools/gen_op_registration.h>
#include <tensorflow/lite/kernels/internal/reference/non_max_suppression.h>

#include <stdexcept>
#include <sstream>

namespace {

} // end of anonymous namespace

using aif::Stopwatch;

namespace aif {

//------------------------------------------------------
// CpuFaceDetector
//------------------------------------------------------
CpuFaceDetector::CpuFaceDetector(const std::string& modelPath)
    : FaceDetector(modelPath)
{
}

CpuFaceDetector::~CpuFaceDetector()
{
}

t_aif_status CpuFaceDetector::compileModel()/* override*/
{
    Logi("Compile Model: CpuFaceDetector");
    std::stringstream errlog;
    try {
        TfLiteStatus res = kTfLiteError;
        if (!m_param->getUseXnnpack()) {
            Logi("Not use xnnpack: BuiltinOpResolverWithoutDefaultDelegates");
            tflite::ops::builtin::BuiltinOpResolverWithoutDefaultDelegates resolver;
            res = tflite::InterpreterBuilder(*m_model.get(), resolver)(&m_interpreter, m_param->getNumThreads());
        } else {
            Logi("Use xnnpack: BuiltinOpResolver");
            tflite::ops::builtin::BuiltinOpResolver resolver;
            res = tflite::InterpreterBuilder(*m_model.get(), resolver)(&m_interpreter, m_param->getNumThreads());
        }

        if (res != kTfLiteOk || m_interpreter == nullptr) {
            throw std::runtime_error("tflite interpreter build failed!!");
        }
        return kAifOk;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
}

t_aif_status CpuFaceDetector::fillInputTensor(const cv::Mat& img)/* override*/
{
    try {
        if (img.rows == 0 || img.cols == 0) {
            throw std::runtime_error("invalid opencv image!!");
        }

        int height = m_modelInfo.height;
        int width = m_modelInfo.width;
        int channels = m_modelInfo.channels;

        if (m_interpreter == nullptr) {
            throw std::runtime_error("tflite interpreter not initialized!!");
        }

        t_aif_status res = aif::fillInputTensor<float, cv::Vec3f>(
                                    m_interpreter.get(),
                                    img,
                                    width,
                                    height,
                                    channels,
                                    false,
                                    aif::kAifStandardization
                                );
        if (res != kAifOk) {
            throw std::runtime_error("fillInputTensor failed!!");
        }

        return res;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
}

//------------------------------------------------------
// ShortRangeFaceDetector
//------------------------------------------------------
ShortRangeFaceDetector::ShortRangeFaceDetector()
    : CpuFaceDetector("face_detection_short_range.tflite")
{
}

std::shared_ptr<DetectorParam> ShortRangeFaceDetector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<FaceParam>();
    std::shared_ptr<FaceParam> fparam = std::dynamic_pointer_cast<FaceParam>(param);
    fparam->strides = {8, 16, 16, 16};
    fparam->optAspectRatios = {1.0f};
    fparam->interpolatedScaleAspectRatio = 1.0f;
    fparam->anchorOffsetX = 0.5f;
    fparam->anchorOffsetY = 0.5f;
    fparam->minScale = 0.1484375f;
    fparam->maxScale = 0.75f;
    fparam->reduceBoxesInLowestLayer = false;
    fparam->scoreThreshold = 0.7f;
    fparam->iouThreshold = 0.2f;
    fparam->maxOutputSize = 100;
    fparam->updateThreshold = 0.3f;

    return param;
}

ShortRangeFaceDetector::~ShortRangeFaceDetector()
{
}

//------------------------------------------------------
// FullRangeFaceDetector
//------------------------------------------------------
FullRangeFaceDetector::FullRangeFaceDetector()
    : CpuFaceDetector("face_detection_full_range.tflite")
{
}

FullRangeFaceDetector::~FullRangeFaceDetector()
{
}

std::shared_ptr<DetectorParam> FullRangeFaceDetector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<FaceParam>();
    std::shared_ptr<FaceParam> fparam = std::dynamic_pointer_cast<FaceParam>(param);
    fparam->strides = {4};
    fparam->optAspectRatios = {1.0f};
    fparam->interpolatedScaleAspectRatio = 0.0f;
    fparam->anchorOffsetX = 0.5f;
    fparam->anchorOffsetY = 0.5f;
    fparam->minScale = 0.1484375f;
    fparam->maxScale = 0.75f;
    fparam->reduceBoxesInLowestLayer = false;
    fparam->scoreThreshold = 0.7f;
    fparam->iouThreshold = 0.2f;
    fparam->maxOutputSize = 100;
    fparam->updateThreshold = 0.3f;

    return param;
}

} // end of namespace aif
