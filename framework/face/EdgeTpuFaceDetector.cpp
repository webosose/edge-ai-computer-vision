#include <aif/face/EdgeTpuFaceDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/tools/gen_op_registration.h>
#include <tensorflow/lite/kernels/internal/reference/non_max_suppression.h>

#include <stdexcept>
#include <sstream>

#include <edgetpu.h>

namespace {

} // end of anonymous namespace

namespace aif {

//------------------------------------------------------
// EdgeTpuFaceDetector
//------------------------------------------------------
EdgeTpuFaceDetector::EdgeTpuFaceDetector(const std::string& modelPath)
    : FaceDetector(modelPath)
{
}

EdgeTpuFaceDetector::~EdgeTpuFaceDetector()
{
    // Releases interpreter instance before the EdgeTpuContext is destroyed.
    // the lifetime of EdgeTpuContext should be longer than all associated interpreter instances.
    m_interpreter.reset();
}

t_aif_status EdgeTpuFaceDetector::compileModel()/* override*/
{
    Logi("Compile Model: EdgeTpuFaceDetector");
    std::stringstream errlog;
    try {
        TfLiteStatus res = kTfLiteError;
        tflite::ops::builtin::BuiltinOpResolver resolver;

        // Sets up the edgetpu_context. available for any 1 TPU device.
        m_edgetpuContext = edgetpu::EdgeTpuManager::GetSingleton()->OpenDevice();
        if (m_edgetpuContext == nullptr) {
            throw std::runtime_error("can't get edgetpu context!!");
        }

        // Registers Edge TPU custom op handler with Tflite resolver.
        resolver.AddCustom(edgetpu::kCustomOp, edgetpu::RegisterCustomOp());

        res = tflite::InterpreterBuilder(*m_model.get(), resolver)(&m_interpreter);
        if (res != kTfLiteOk || m_interpreter == nullptr) {
            throw std::runtime_error("tflite interpreter build failed!!");
        }

        m_interpreter->SetExternalContext(kTfLiteEdgeTpuContext, m_edgetpuContext.get());
        return kAifOk;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
}

t_aif_status EdgeTpuFaceDetector::fillInputTensor(const cv::Mat& img)/* override*/
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

        t_aif_status res = aif::fillInputTensor<uint8_t, cv::Vec3b>(
                                    m_interpreter.get(),
                                    img,
                                    width,
                                    height,
                                    channels,
                                    true,
                                    aif::kAifNone
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
// EdgeTpuShortRangeFaceDetector
//------------------------------------------------------
EdgeTpuShortRangeFaceDetector::EdgeTpuShortRangeFaceDetector()
    : EdgeTpuFaceDetector("/usr/share/aif/model/face-detector-quantized_edgetpu.tflite")
{
}

EdgeTpuShortRangeFaceDetector::~EdgeTpuShortRangeFaceDetector()
{
}

std::shared_ptr<DetectorParam> EdgeTpuShortRangeFaceDetector::createParam()
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

} // end of namespace aif
