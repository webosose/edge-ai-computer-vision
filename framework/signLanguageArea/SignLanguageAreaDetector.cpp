/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/signLanguageArea/SignLanguageAreaDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>
#include <vector>

namespace aif {

SignLanguageAreaDetector::SignLanguageAreaDetector(const std::string& modelPath)
    : Detector(modelPath)
{
}

SignLanguageAreaDetector::~SignLanguageAreaDetector()
{
}

std::shared_ptr<DetectorParam> SignLanguageAreaDetector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<SignLanguageAreaParam>();
    return param;
}

void SignLanguageAreaDetector::setModelInfo(TfLiteTensor* inputTensor)
{
    if (inputTensor == nullptr) {
        Loge(__func__, "input tensor is nullptr");
        return;
    }

    m_modelInfo.batchSize = inputTensor->dims->data[0];
    m_modelInfo.height = inputTensor->dims->data[1];
    m_modelInfo.width = inputTensor->dims->data[2];
    m_modelInfo.channels = inputTensor->dims->data[3];

    TRACE("batch_size: ", m_modelInfo.batchSize);
    TRACE("height:", m_modelInfo.height);
    TRACE("width: ", m_modelInfo.width);
    TRACE("channels: ", m_modelInfo.channels);
}

t_aif_status SignLanguageAreaDetector::fillInputTensor(const cv::Mat& img)/* override*/
{
    if (img.rows == 0 || img.cols == 0) {
        Loge("invalid opencv image!!");
        return kAifError;
    }

    if (m_interpreter == nullptr) {
        Loge("tflite interpreter not initialized!!");
        return kAifError;
    }

    float* inputTensor = m_interpreter->typed_input_tensor<float>(0);
    if (inputTensor == nullptr) {
        Loge(__func__, "failed to get inputTensor pointer");
        return kAifError;
    }

    t_aif_status res = aif::fillInputTensor<float, cv::Vec3f>(
            m_interpreter.get(), img, m_modelInfo.width, m_modelInfo.height, m_modelInfo.channels, false,
            aif::kAifStandardization);
    if (res != kAifOk) {
        Loge("fillInputTensor failed!!");
    }

    return res;
}

t_aif_status SignLanguageAreaDetector::preProcessing()
{
    return kAifOk;
}

t_aif_status SignLanguageAreaDetector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    std::shared_ptr<SignLanguageAreaDescriptor> signLanguageAreaDescriptor =
        std::dynamic_pointer_cast<SignLanguageAreaDescriptor>(descriptor);
    if (nullptr == signLanguageAreaDescriptor) {
        Loge("casted pointer returned null!");
        return kAifError;
    }

    std::vector<float> results;

    const std::vector<int> &outputs = m_interpreter->outputs();
    if (2 > outputs.size()) {
        signLanguageAreaDescriptor->addBoxes(results);
        return kAifOk;
    }

    constexpr int conf_index = 0;
    constexpr int bbox_index = 1;

    const auto conf_length = m_interpreter->tensor(outputs[conf_index])->dims->data[1];
    const auto num_conf_classes = m_interpreter->tensor(outputs[conf_index])->dims->data[2];

    const auto bbox_length = m_interpreter->tensor(outputs[bbox_index])->dims->data[1];
    const auto num_bbox_coord_pos = m_interpreter->tensor(outputs[bbox_index])->dims->data[2];

    if (conf_length != bbox_length || 2 != num_conf_classes || 4 != num_bbox_coord_pos) {
        Loge("size check failed!! ", conf_length, " ", num_conf_classes, " ", bbox_length, " ", num_bbox_coord_pos);
        signLanguageAreaDescriptor->addBoxes(results);
        return kAifOk;
    }

    float *conf = m_interpreter->tensor(outputs[conf_index])->data.f;
    float *bbox = m_interpreter->tensor(outputs[bbox_index])->data.f;

    float confidence_threshold = 0.f;
    results.resize(5, 0.f);

    bool first_val = true;
    for (int i = 0; i < conf_length; ++i) {
        // class #0 in mb2-ssd-lite is BACKGROUND (ignored)
        const float score = conf[i * num_conf_classes + 1];
        if (first_val) {
            confidence_threshold = score - 0.1f;
            first_val = false;
        }

        if (score < confidence_threshold)
            continue;

        confidence_threshold = score;

        for (int j = 0; j < num_bbox_coord_pos; ++j)
            results[j] = bbox[i * num_bbox_coord_pos + j];
        results[num_bbox_coord_pos] = score;
    }

    signLanguageAreaDescriptor->addBoxes(results);

    return kAifOk;
}

} // end of namespace aif
