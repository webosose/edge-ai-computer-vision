#include <aif/poseLandmark/CpuPoseLandmarkDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/tools/gen_op_registration.h>
#include <tensorflow/lite/kernels/internal/reference/non_max_suppression.h>

#include <stdexcept>
#include <sstream>

namespace aif {

CpuPoseLandmarkDetector::CpuPoseLandmarkDetector(
        const std::string& modelPath,
        const std::shared_ptr<DetectorParam>& param)
    : PoseLandmarkDetector(modelPath, param)
    , m_delegateProvider(std::make_unique<XnnpackDelegate>())
    , m_useXnnpack(true)
    , m_numThreads(0)
{
}

CpuPoseLandmarkDetector::~CpuPoseLandmarkDetector()
{
}

t_aif_status CpuPoseLandmarkDetector::setOptions(const std::string& options) /* override */
{
    return m_delegateProvider->setXnnpackDelegateOptions(options, m_useXnnpack, m_numThreads);
}

t_aif_status CpuPoseLandmarkDetector::compileModel()/* override*/
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
        std::unique_ptr<tflite::OpResolver> resolver;
        if (m_useXnnpack) {
            resolver = std::make_unique<tflite::ops::builtin::BuiltinOpResolver>();
        } else {
            resolver = std::make_unique<tflite::ops::builtin::BuiltinOpResolverWithoutDefaultDelegates>();
        }

        res = tflite::InterpreterBuilder(*m_model.get(), *resolver.get())(&m_interpreter, m_numThreads);
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


LiteCpuPoseLandmarkDetector::LiteCpuPoseLandmarkDetector()
    : CpuPoseLandmarkDetector(
        "/usr/share/aif/model/pose_landmark_lite.tflite",
         std::make_shared<PoseLandmarkParam>())
{
}

HeavyCpuPoseLandmarkDetector::HeavyCpuPoseLandmarkDetector()
    : CpuPoseLandmarkDetector(
        "/usr/share/aif/model/pose_landmark_heavy.tflite",
        std::make_shared<PoseLandmarkParam>())
{
}

FullCpuPoseLandmarkDetector::FullCpuPoseLandmarkDetector()
    : CpuPoseLandmarkDetector(
        "/usr/share/aif/model/pose_landmark_full.tflite",
        std::make_shared<PoseLandmarkParam>())
{
}


}
