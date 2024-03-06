/*
 * Copyright (c) 2022-2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/DelegateFactory.h>
#include <aif/base/Detector.h>
#include <aif/log/Logger.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

using aif::Stopwatch;

namespace aif {

Detector::Detector(const std::string &modelName)
    : m_modelName(modelName), m_model(nullptr), m_interpreter(nullptr),
      m_param(nullptr), m_autoDelegateMode(false) {
    memset(&m_modelInfo, 0, sizeof(m_modelInfo));
}

Detector::~Detector() {
    PerformanceReporter::get().removeRecorder(m_modelName);
}

t_aif_status Detector::init(const std::string &param) {
    m_performance = std::make_shared<PerformanceRecorder>(m_modelName, param);
    PerformanceReporter::get().addRecorder(m_modelName, m_performance);
    m_performance->start(Performance::CREATE_DETECTOR);

    m_param = createParam();
    if (!param.empty() && m_param->fromJson(param) != kAifOk) {
        Loge("failed to read param from json: ", m_modelName, " / param: ", param);
        return kAifError;
    }

    if (compile() != kAifOk) {
        Loge("tflite model compile failed: ", m_modelName);
        return kAifError;
    }
    t_aif_status status = preProcessing();
    m_performance->stop(Performance::CREATE_DETECTOR);

    return status;
}

void Detector::setModelInfo(TfLiteTensor* inputTensor)
{
    t_aif_modelinfo inputInfo;
    memset(&inputInfo, 0, sizeof(inputInfo));

    inputInfo.inputSize = inputTensor->dims->size;   // inputSize
    if (inputTensor->dims->data[0] < 1) {            // batchSize
        inputInfo.batchSize = 1;
    } else {
        inputInfo.batchSize = inputTensor->dims->data[0];
    }
    inputInfo.height = inputTensor->dims->data[1];   // height
    inputInfo.width = inputTensor->dims->data[2];    // width
    inputInfo.channels = inputTensor->dims->data[3]; // channels

    TRACE("input_size: ", inputInfo.inputSize);
    TRACE("batch_size: ", inputInfo.batchSize);
    TRACE("height:", inputInfo.height);
    TRACE("width: ", inputInfo.width);
    TRACE("channels: ", inputInfo.channels);

    m_modelInputInfo.emplace_back(inputInfo);
    if (m_modelInputInfo.size() > 0) {
        m_modelInfo = m_modelInputInfo[0];
    }
}

void Detector::setModelInOutInfo(const std::vector<int> &t_inputs,
                                 const std::vector<int> &t_outputs)
{
    // get TfLiteTensor like below.
    // TfLiteTensor *tensor_input = m_interpreter->tensor(t_inputs[i]);
    // TfLiteTensor *tensor_output = m_interpreter->tensor(t_outputs[i]);
}

std::string Detector::getModelPath() const
{
    return AIVision::getModelPath(m_modelName);
}

t_aif_status Detector::compile() {
    std::string path = getModelPath();
    m_model = tflite::FlatBufferModel::BuildFromFile(path.c_str());
    if (m_model == nullptr) {
        Loge("Can't get tflite model: ",  path);
        return kAifError;
    }

    Logi("compile: ", path);

    tflite::ops::builtin::BuiltinOpResolver resolver;
    if (compileModel(resolver) != kAifOk) {
        Loge("tflite model compile failed: ", path);
        return kAifError;
    }

    if (m_interpreter->SetNumThreads(m_param->getNumThreads()) != kTfLiteOk) {
        Loge("failed to set num threads : ", m_param->getNumThreads());
        return kAifError;
    }

    Logi("set num threads : ", m_param->getNumThreads());
    if (compileDelegates(resolver) != kAifOk) {
        Loge("tflite model compile delegates failed: ", path);
        return kAifError;
    }

    TfLiteStatus res = kTfLiteError;
    res = m_interpreter->AllocateTensors();
    if (res != kTfLiteOk) {
        Loge("tflite allocate tensors failed!!");
        return kAifError;
    }

    const std::vector<int> &t_outputs = m_interpreter->outputs();
    const std::vector<int> &t_inputs = m_interpreter->inputs();
    for(int i=0; i < t_inputs.size(); i++){
        TfLiteTensor *tensor_input = m_interpreter->tensor(t_inputs[i]);
        if (tensor_input == nullptr || tensor_input->dims == nullptr) {
            Loge("tflite tensor_input invalid!!");
            return kAifError;
        }

        if (tensor_input->dims == nullptr) {
            Loge("invalid tensor_input dimension!");
            return kAifError;
        }

        m_modelInfo.inputSize = tensor_input->dims->size;
        if (m_modelInfo.inputSize <= 0) {
            Loge("this model input require > 0 tensors");
            return kAifError;
        }
        setModelInfo(tensor_input);
    }
#if 1
    setModelInOutInfo(t_inputs, t_outputs);
#endif

    return kAifOk;
}

t_aif_status
Detector::compileModel(tflite::ops::builtin::BuiltinOpResolver &resolver) {
    TfLiteStatus res = tflite::InterpreterBuilder(*m_model.get(), resolver)(
            &m_interpreter, MAX_INTERPRETER_THREADS);

    if (res != kTfLiteOk || m_interpreter == nullptr) {
        Loge("tflite interpreter build failed!!");
        return kAifError;
    }
    return kAifOk;
}

t_aif_status
Detector::compileDelegates(tflite::ops::builtin::BuiltinOpResolver &resolver) {
    if (m_param->getUseAutoDelegate()) {
#ifdef USE_AUTO_DELEGATE
        m_ads = std::make_unique<AutoDelegateSelector>();
        AccelerationPolicyManager apm(m_param->getAutoDelegateConfig());
        m_autoDelegateMode = m_ads->selectDelegate(*m_interpreter.get(), apm);
        if (m_autoDelegateMode) {
            Logi("auto delegate mode on");
        } else {
            Loge("auto delegate mode off(Failed to select auto delegates)");
        }
#endif
        return kAifOk;
    }

    auto &delegates = m_param->getDelegates();
    for (auto &delegate : delegates) {
        m_delegates.push_back(DelegateFactory::get().getDelegate(
            delegate.first, delegate.second));
        Logi("delegate: ", delegate.first, "/ option: ", delegate.second);
    }

    if (m_delegates.size() == 0) {
        return kAifOk;
    }

    TfLiteStatus res = kTfLiteError;
    for (auto &delegate : m_delegates) {
        if (delegate == nullptr)
            continue;

        res = m_interpreter->ModifyGraphWithDelegate(
                delegate->getTfLiteDelegate());
        if (res == kTfLiteOk) {
            Logi("selected delegate: ", delegate->getName());
            return kAifOk;
        } else {
            Logw(__func__,
                    "Error: modify graph failed with " + delegate->getName());
        }
    }
    Loge("tflite Modify failed");
    return kAifError;
}

t_aif_status
Detector::detectFromImage(const std::string &imagePath,
        std::shared_ptr<Descriptor> &descriptor) {
    Stopwatch sw;
    cv::Mat img;
    // prepare input tensors
    sw.start();
    if (aif::getCvImageFrom(imagePath, img) != kAifOk) {
        Loge("can't get opencv image: ", imagePath);
        return kAifError;
    }
    TRACE("aif::getCvImageFrom(): ", sw.getMs(), "ms");
    sw.stop();


    return detect(img, descriptor);
}

t_aif_status
Detector::detectFromBase64(const std::string &base64image,
        std::shared_ptr<Descriptor> &descriptor) {
    Stopwatch sw;
    cv::Mat img;
    // prepare input tensors
    sw.start();
    if (aif::getCvImageFromBase64(base64image, img) != kAifOk) {
        Loge("can't get opencv image from base64image!");
        sw.stop();
        return kAifError;
    }
    TRACE("aif::getCvImageFromBase64(): ", sw.getMs(), "ms");
    sw.stop();

    return detect(img, descriptor);
}

t_aif_status Detector::detect(const cv::Mat &img,
        std::shared_ptr<Descriptor> &descriptor) {
    // pre-process
    m_performance->start(Performance::PREPROCESS);
    if (fillInputTensor(img) != kAifOk) {
        Loge("fill input tensor failed!!");
        m_performance->stop(Performance::PREPROCESS);
        return kAifError;
    }
    m_performance->stop(Performance::PREPROCESS);

    // inference
    m_performance->start(Performance::PROCESS);
    TfLiteStatus res = m_interpreter->Invoke();
    if (res != kTfLiteOk) {
        Loge("tflite interpreter invoke failed!!");
        m_performance->stop(Performance::PROCESS);
        return kAifError;
    }
    m_performance->stop(Performance::PROCESS);

    // post-process
    m_performance->start(Performance::POSTPROCESS);
    t_aif_status status = postProcessing(img, descriptor);
    m_performance->stop(Performance::POSTPROCESS);

    return status;
}

} // end of namespace aif
