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

ArmNNPosenetDetector::ArmNNPosenetDetector(
    const std::string& model_path,
    const std::string& options
)   : PosenetDetector(
            model_path, //"/usr/share/aif/model/posenet_mobilenet_v1_075_353_481_quant_decoder.tflite",
            std::make_shared<PosenetParam>())
    , m_Options(options)
    , m_delegate(nullptr, armnnDelegate::TfLiteArmnnDelegateDelete)
{
}

ArmNNPosenetDetector::~ArmNNPosenetDetector()
{
}


std::vector<std::string> ArmNNPosenetDetector::splitString(const std::string& str, const char delim)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream ss(str);
    while (std::getline(ss, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}

bool ArmNNPosenetDetector::parseDelegateOptions(armnnDelegate::DelegateOptions& delegateOptions)
{

    const std::vector<std::string> options = splitString(m_Options, ';');
    std::vector<std::string> keys, values;

    for (const auto& option: options) {
        auto key_value = splitString(option, ':');
        if (key_value.size() != 2) {
            std::cout << "option string is wrong..." << std::endl;
            return false;
        }
        keys.emplace_back(key_value[0]);
        values.emplace_back(key_value[1]);
    }

    for (auto i=0; i < options.size(); i++) {
	std::cout << "key:" << keys[i] << ", value:" << values[i] << std::endl;
        if (keys[i] == std::string("backends")) {
            std::vector<armnn::BackendId> backends;
            auto backendsStr = splitString(values[i], ',');
            for (const auto& backend : backendsStr) {
                backends.push_back(backend);
            }
            delegateOptions.SetBackends(backends);
        }
        if (keys[i] == std::string("logging-severity")) {
            delegateOptions.SetLoggingSeverity(values[i]);
        }
        // TODO: other options....
    }

    return true;
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
        auto delegateOptions = armnnDelegate::TfLiteArmnnDelegateOptionsDefault();
        parseDelegateOptions(delegateOptions);
        m_delegate.reset(armnnDelegate::TfLiteArmnnDelegateCreate(delegateOptions));
        // Instruct the Interpreter to use the armnnDelegate
        m_interpreter->ModifyGraphWithDelegate(m_delegate.get());

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
