#include <aif/pose/ArmNNPosenetDetector.h>
#include <aif/pose/posenet_decoder_op.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/tools/gen_op_registration.h>
#include <tensorflow/lite/kernels/internal/reference/non_max_suppression.h>

#include <stdexcept>
#include <sstream>
#include <vector>

namespace aif {

ArmNNPosenetDetector::ArmNNPosenetDetector()
    : PosenetDetector(
        "/usr/share/aif/model/posenet_mobilenet_v1_075_353_481_quant_decoder.tflite",
        std::make_shared<PosenetParam>())
    , m_delegateProvider(std::make_unique<ArmNNDelegate>())
    , m_delegatePtr(nullptr, armnnDelegate::TfLiteArmnnDelegateDelete)
{
}

ArmNNPosenetDetector::~ArmNNPosenetDetector()
{
}

t_aif_status ArmNNPosenetDetector::setOptions(const std::string& options) /*override;*/
{
    return m_delegateProvider->setArmNNDelegateOptions(options);
}

t_aif_status ArmNNPosenetDetector::compileModel()/* override*/
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
        resolver.AddCustom(coral::kPosenetDecoderOp, coral::RegisterPosenetDecoderOp());
        res = tflite::InterpreterBuilder(*m_model.get(), resolver)(&m_interpreter);
        if (res != kTfLiteOk || m_interpreter == nullptr) {
            throw std::runtime_error("tflite interpreter build failed!!");
        }

        // Create the Arm NN Delegate
        m_delegatePtr = m_delegateProvider->createArmNNDelegate();
        // Instruct the Interpreter to use the armnnDelegate
        m_interpreter->ModifyGraphWithDelegate(m_delegatePtr.get());

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

}
