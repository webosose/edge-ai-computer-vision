/*
 * Copyright (c) 2022-2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_DETECTOR_H
#define AIF_DETECTOR_H

#include <aif/base/Delegate.h>
#include <aif/base/Descriptor.h>
#include <aif/base/DetectorParam.h>
#include <aif/base/Types.h>
#include <aif/tools/PerformanceReporter.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/model.h>
#include <tensorflow/lite/tools/gen_op_registration.h>

#ifdef USE_AUTO_DELEGATE
#include <aif/auto_delegation/AccelerationPolicyManager.h>
#include <aif/auto_delegation/AutoDelegateSelector.h>
#endif


namespace aif {

class Detector {
  public:
    Detector(const std::string &modelName);
    virtual ~Detector();

    enum { MAX_INTERPRETER_THREADS = 4 };

    virtual t_aif_status init(const std::string &param = "");
    virtual t_aif_status
    detectFromImage(const std::string &imagePath,
                    std::shared_ptr<Descriptor> &descriptor);
    virtual t_aif_status
    detectFromBase64(const std::string &base64image,
                     std::shared_ptr<Descriptor> &descriptor);
    virtual t_aif_status detect(const cv::Mat &img,
                                std::shared_ptr<Descriptor> &descriptor);

    const std::string &getModelName() const { return m_modelName; }
    const t_aif_modelinfo &getModelInfo() const { return m_modelInfo; }
    const std::vector<t_aif_modelinfo> &getModelInputInfo() const { return m_modelInputInfo; }
    std::shared_ptr<DetectorParam> getParam() const { return m_param; }
    size_t getNumDelegates() const { return m_delegates.size(); }
    bool getAutoDelegateMode() const { return m_autoDelegateMode; }
    std::string getModelPath() const;

  protected:
    virtual std::shared_ptr<DetectorParam> createParam() = 0;
    virtual t_aif_status fillInputTensor(const cv::Mat &img) = 0;
    virtual t_aif_status preProcessing() = 0;
    virtual t_aif_status
    postProcessing(const cv::Mat &img,
                   std::shared_ptr<Descriptor> &descriptor) = 0;

    virtual void setModelInfo(TfLiteTensor* inputTensor);
    virtual t_aif_status compile();
    virtual t_aif_status
    compileModel(tflite::ops::builtin::BuiltinOpResolver &resolver);
    virtual t_aif_status
    compileDelegates(tflite::ops::builtin::BuiltinOpResolver &resolver);

  protected:
    std::string m_modelName;
    t_aif_modelinfo m_modelInfo;                    // first input tensor info
    std::vector<t_aif_modelinfo> m_modelInputInfo;  // all   input tensor info
    std::unique_ptr<tflite::FlatBufferModel> m_model;
    std::unique_ptr<tflite::Interpreter> m_interpreter;
    std::shared_ptr<DetectorParam> m_param;
    std::vector<std::shared_ptr<Delegate>> m_delegates;
    bool m_autoDelegateMode;
    std::shared_ptr<PerformanceRecorder> m_performance;
#ifdef USE_AUTO_DELEGATE
    std::unique_ptr<AutoDelegateSelector> m_ads;
#endif
};

} // end of namespace aif

#endif // AIF_DETECTOR_H
