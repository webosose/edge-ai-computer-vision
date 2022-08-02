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

#ifdef USE_AUTO_DELEGATE
#include <aif/auto_delegation/AccelerationPolicyManager.h>
#include <aif/auto_delegation/AutoDelegateSelector.h>
#endif

using aif::Stopwatch;

namespace aif {

Detector::Detector(const std::string &modelName)
    : m_modelName(modelName), m_model(nullptr), m_interpreter(nullptr),
      m_param(nullptr), m_autoDelegateMode(false) {
    memset(&m_modelInfo, 0, sizeof(m_modelInfo));
}

Detector::~Detector() {}

t_aif_status Detector::init(const std::string &param) {
    std::stringstream errlog;
    try {
        m_performance = std::make_shared<PerformanceRecorder>(m_modelName, param);
        PerformanceReporter::get().addRecorder(m_performance);

        m_performance->start(Performance::CREATE_DETECTOR);
        m_param = createParam();
        if (!param.empty() && m_param->fromJson(param) != kAifOk) {
            errlog.clear();
            errlog << "failed to read param from json: " << m_modelName << "\n"
                   << param;
            throw std::runtime_error(errlog.str());
        }

        if (compile() != kAifOk) {
            errlog.clear();
            errlog << "tflite model compile failed: " << m_modelName;
            throw std::runtime_error(errlog.str());
        }
        t_aif_status status = preProcessing();
        m_performance->stop(Performance::CREATE_DETECTOR);

        return status;
    } catch (const std::exception &e) {
        Loge(__func__, "Error: ", e.what());
        return kAifError;
    } catch (...) {
        Loge(__func__, "Error: Unknown exception occured!!");
        return kAifError;
    }
}

void Detector::setModelInfo(TfLiteTensor* inputTensor)
{
    m_modelInfo.batchSize = inputTensor->dims->data[0];
    m_modelInfo.height = inputTensor->dims->data[1];
    m_modelInfo.width = inputTensor->dims->data[2];
    m_modelInfo.channels = inputTensor->dims->data[3];

    TRACE("input_size: ", m_modelInfo.inputSize);
    TRACE("batch_size: ", m_modelInfo.batchSize);
    TRACE("height:", m_modelInfo.height);
    TRACE("width: ", m_modelInfo.width);
    TRACE("channels: ", m_modelInfo.channels);
}

std::string Detector::getModelPath() const
{
    return AIVision::getModelPath(m_modelName);
}

t_aif_status Detector::compile() {
    std::stringstream errlog;
    try {
        std::string path = getModelPath();
        m_model = tflite::FlatBufferModel::BuildFromFile(path.c_str());
        if (m_model == nullptr) {
            errlog.clear();
            errlog << "Can't get tflite model: " << path;
            throw std::runtime_error(errlog.str());
        }

        Logi("compile: ", path);

        // TODO: if not use XNNPACK,
        // use tflite::ops::builtin::BuiltinOpResolverWithoutDefaultDelegates
        tflite::ops::builtin::BuiltinOpResolver resolver;
        if (compileModel(resolver) != kAifOk) {
            errlog.clear();
            errlog << "tflite model compile failed: " << path;
            throw std::runtime_error(errlog.str());
        }

        if (m_interpreter->SetNumThreads(m_param->getNumThreads()) !=
            kTfLiteOk) {
            Loge("failed to set num threads : ", m_param->getNumThreads());
        } else {
            Logi("set num threads : ", m_param->getNumThreads());
        }

        if (compileDelegates(resolver) != kAifOk) {
            errlog.clear();
            errlog << "tflite model compile delegates failed: " << path;
            throw std::runtime_error(errlog.str());
        }

        TfLiteStatus res = kTfLiteError;
        res = m_interpreter->AllocateTensors();
        if (res != kTfLiteOk) {
            throw std::runtime_error("tflite allocate tensors failed!!");
        }

        const std::vector<int> &t_inputs = m_interpreter->inputs();
        TfLiteTensor *tensor_input = m_interpreter->tensor(t_inputs[0]);
        if (tensor_input == nullptr || tensor_input->dims == nullptr) {
            throw std::runtime_error("tflite tensor_input invalid!!");
        }

        if (tensor_input->dims == nullptr) {
            throw std::runtime_error("invalid tensor_input dimension!");
        }

        m_modelInfo.inputSize = tensor_input->dims->size;
        if (m_modelInfo.inputSize <= 0) {
            throw std::runtime_error("this model input require >0 tensors");
        }
        setModelInfo(tensor_input);

        return kAifOk;
    } catch (const std::exception &e) {
        Loge(__func__, "Error: ", e.what());
        return kAifError;
    } catch (...) {
        Loge(__func__, "Error: Unknown exception occured!!");
        return kAifError;
    }
}

t_aif_status
Detector::compileModel(tflite::ops::builtin::BuiltinOpResolver &resolver) {
    std::stringstream errlog;
    try {
        TfLiteStatus res = kTfLiteError;
        res = tflite::InterpreterBuilder(*m_model.get(), resolver)(
            &m_interpreter, MAX_INTERPRETER_THREADS);

        if (res != kTfLiteOk || m_interpreter == nullptr) {
            throw std::runtime_error("tflite interpreter build failed!!");
        }
        return kAifOk;
    } catch (const std::exception &e) {
        Loge(__func__, "Error: ", e.what());
        return kAifError;
    } catch (...) {
        Loge(__func__, "Error: Unknown exception occured!!");
        return kAifError;
    }
}

t_aif_status
Detector::compileDelegates(tflite::ops::builtin::BuiltinOpResolver &resolver) {
    if (m_param->getUseAutoDelegate()) {
#ifdef USE_AUTO_DELEGATE
        AutoDelegateSelector ads;
        AccelerationPolicyManager apm(m_param->getAutoDelegateConfig());
        m_autoDelegateMode = ads.selectDelegate(*m_interpreter.get(), apm);
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

    try {
        TfLiteStatus res = kTfLiteError;
        for (auto &delegate : m_delegates) {
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
        throw std::runtime_error("tflite Modify failed");
        return kAifError;
    } catch (const std::exception &e) {
        Loge(__func__, "Error: ", e.what());
        return kAifError;
    } catch (...) {
        Loge(__func__, "Error: Unknown exception occured!!");
        return kAifError;
    }
}

t_aif_status
Detector::detectFromImage(const std::string &imagePath,
                          std::shared_ptr<Descriptor> &descriptor) {
    try {
        Stopwatch sw;
        cv::Mat img;
        // prepare input tensors
        sw.start();
        if (aif::getCvImageFrom(imagePath, img) != kAifOk) {
            throw std::runtime_error(std::string("can't get opencv image: ") +
                                     imagePath);
        }
        TRACE("aif::getCvImageFrom(): ", sw.getMs(), "ms");
        sw.stop();


        return detect(img, descriptor);
    } catch (const std::exception &e) {
        Loge(__func__, "Error: ", e.what());
        return kAifError;
    } catch (...) {
        Loge(__func__, "Error: Unknown exception occured!!");
        return kAifError;
    }
}

t_aif_status
Detector::detectFromBase64(const std::string &base64image,
                           std::shared_ptr<Descriptor> &descriptor) {
    try {
        Stopwatch sw;
        cv::Mat img;
        // prepare input tensors
        sw.start();
        if (aif::getCvImageFromBase64(base64image, img) != kAifOk) {
            throw std::runtime_error(
                "can't get opencv image from base64image!");
        }
        TRACE("aif::getCvImageFromBase64(): ", sw.getMs(), "ms");
        sw.stop();

        return detect(img, descriptor);
    } catch (const std::exception &e) {
        Loge(__func__, "Error: ", e.what());
        return kAifError;
    } catch (...) {
        Loge(__func__, "Error: Unknown exception occured!!");
        return kAifError;
    }
}

t_aif_status Detector::detect(const cv::Mat &img,
                              std::shared_ptr<Descriptor> &descriptor) {
    try {
        // pre-process
        m_performance->start(Performance::PREPROCESS);
        if (fillInputTensor(img) != kAifOk) {
            throw std::runtime_error("fill input tensor failed!!");
        }
        m_performance->stop(Performance::PREPROCESS);

        // inference
        m_performance->start(Performance::PROCESS);
        TfLiteStatus res = m_interpreter->Invoke();
        if (res != kTfLiteOk) {
            throw std::runtime_error("tflite interpreter invoke failed!!");
        }
        m_performance->stop(Performance::PROCESS);

        // post-process
        m_performance->start(Performance::POSTPROCESS);
        t_aif_status status = postProcessing(img, descriptor);
        m_performance->stop(Performance::POSTPROCESS);

        return status;
    } catch (const std::exception &e) {
        Loge(__func__, "Error: ", e.what());
        return kAifError;
    } catch (...) {
        Loge(__func__, "Error: Unknown exception occured!!");
        return kAifError;
    }
}

} // end of namespace aif
