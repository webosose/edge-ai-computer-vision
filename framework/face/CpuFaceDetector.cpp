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
CpuFaceDetector::CpuFaceDetector(
    const std::string& modelPath,
    const std::shared_ptr<DetectorParam>& param)
    : FaceDetector(modelPath, param)
{
}

CpuFaceDetector::~CpuFaceDetector()
{
}

t_aif_status CpuFaceDetector::compileModel()/* override*/
{
    std::stringstream errlog;
    try {
        m_model = tflite::FlatBufferModel::BuildFromFile(m_modelPath.c_str());
        if (m_model == nullptr) {
            errlog.clear();
            errlog << "Can't get tflite model: " << m_modelPath;
            throw std::runtime_error(errlog.str());
        }

        TfLiteStatus res = kTfLiteError;
        tflite::ops::builtin::BuiltinOpResolver resolver;
        res = tflite::InterpreterBuilder(*m_model.get(), resolver)(&m_interpreter);
        if (res != kTfLiteOk || m_interpreter == nullptr) {
            throw std::runtime_error("tflite interpreter build failed!!");
        }

        res = m_interpreter->AllocateTensors();
        if (res != kTfLiteOk) {
            throw std::runtime_error("tflite allocate tensors failed!!");
        }

        const std::vector<int> &t_inputs = m_interpreter->inputs();
        TfLiteTensor* tensor_input = m_interpreter->tensor(t_inputs[0]);
        if (tensor_input == nullptr || tensor_input->dims == nullptr) {
            throw std::runtime_error("tflite tensor_input invalid!!");
        }

        if (tensor_input->dims == nullptr) {
            throw std::runtime_error("invalid tensor_input dimension!");
        }

        m_modelInfo.inputSize = tensor_input->dims->size;
        if (m_modelInfo.inputSize != 4) {
            throw std::runtime_error("this model input require 4 tensors");
        }

        m_modelInfo.batchSize = tensor_input->dims->data[0];
        m_modelInfo.height = tensor_input->dims->data[1];
        m_modelInfo.width = tensor_input->dims->data[2];
        m_modelInfo.channels = tensor_input->dims->data[3];

        TRACE("input_size: ", m_modelInfo.inputSize);
        TRACE("batch_size: ", m_modelInfo.batchSize);
        TRACE("height:", m_modelInfo.height);
        TRACE("width: ", m_modelInfo.width);
        TRACE("channels: ", m_modelInfo.channels);

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

/*
FaceParam(
        const std::vector<int>& strides = {8, 16, 16, 16},
        const std::vector<float>& optAspectRatio = {1.0f},
        float interpolatedScaleAspectRatio = 1.0f,
        float anchorOffsetX = 0.5,
        float anchorOffsetY = 0.5,
        float minScale = 0.1484375,
        float maxScale = 0.75,
        bool  reduceBoxesInLowestLayer = false,
        float scoreThreshold = 0.7f,
        float iouThreshold = 0.2f,
        int   maxOutPutSize = 100
    );
*/
//------------------------------------------------------
// ShortRangeFaceDetector
//------------------------------------------------------
ShortRangeFaceDetector::ShortRangeFaceDetector()
    : CpuFaceDetector(
        "model/face_detection_short_range.tflite",
        std::make_shared<FaceParam>(
            std::vector<int>{8, 16, 16, 16},   // strides
            std::vector<float>{1.0f},          // optAspectRatios
            1.0f,               // interpolatedScaleAspectRatio
            0.5,                // anchorOffsetX
            0.5,                // anchorOffsetY
            0.1484375,          // minScale
            0.75,               // maxScale
            false,              // reduceBoxesInLowestLayer
            0.7f,               // scoreThreshold
            0.2f,               // iouThreshold
            100                 // maxOutputSize
        ))
{
}

ShortRangeFaceDetector::~ShortRangeFaceDetector()
{
}

//------------------------------------------------------
// FullRangeFaceDetector
//------------------------------------------------------
FullRangeFaceDetector::FullRangeFaceDetector()
    : CpuFaceDetector(
        "model/face_detection_full_range.tflite",
        std::make_shared<FaceParam>(
            std::vector<int>{4},        // strides
            std::vector<float>{1.0f},   // optAspectRatios
            0.0f,               // interpolatedScaleAspectRatio
            0.5,                // anchorOffsetX
            0.5,                // anchorOffsetY
            0.1484375,          // minScale
            0.75,               // maxScale
            false,              // reduceBoxesInLowestLayer
            0.7f,               // scoreThreshold
            0.2f,               // iouThreshold
            100                 // maxOutputSize
        ))
{
}

FullRangeFaceDetector::~FullRangeFaceDetector()
{
}

} // end of namespace aif
