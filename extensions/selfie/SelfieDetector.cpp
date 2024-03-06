/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/selfie/SelfieDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

namespace aif {

SelfieDetector::SelfieDetector(const std::string& modelPath)
    : Detector(modelPath)
{
}

SelfieDetector::~SelfieDetector()
{
}

std::shared_ptr<DetectorParam> SelfieDetector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<SelfieParam>();
    return param;
}

t_aif_status SelfieDetector::fillInputTensor(const cv::Mat& img)/* override*/
{
    if (img.rows == 0 || img.cols == 0) {
        Loge("invalid opencv image!!");
        return kAifError;
    }

    int height = m_modelInfo.height;
    int width = m_modelInfo.width;
    int channels = m_modelInfo.channels;

    if (m_interpreter == nullptr) {
        Loge("tflite interpreter not initialized!!");
        return kAifError;
    }

    t_aif_status res = aif::fillInputTensor<float, cv::Vec3f>(
            m_interpreter.get(),
            img,
            width,
            height,
            channels,
            false,
            aif::kAifNormalization
            );
    if (res != kAifOk) {
        Loge("fillInputTensor failed!!");
        return kAifError;
    }

    return res;
}

t_aif_status SelfieDetector::preProcessing()
{
    return kAifOk;
}

void SelfieDetector::printOutput() {
    const std::vector<int>& outputs = m_interpreter->outputs();
    for (int i = 0; i < outputs.size(); i++) {
        TfLiteTensor *output= m_interpreter->tensor(outputs[i]);
        TRACE(i, ":output bytes: ", output->bytes);
        TRACE(i, ":output type: ", output->type);
        TRACE(i, ":output dims 0: ", output->dims->data[0]);
        TRACE(i, ":output dims 1: ",  output->dims->data[1]);
        TRACE(i, ":output dims 2: ",  output->dims->data[2]);
        TRACE(i, ":output dims 3: ",  output->dims->data[3]);
    }
}

t_aif_status SelfieDetector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    //printOutput();
    printOutput();
    const std::vector<int> &t_outputs = m_interpreter->outputs();
    TfLiteTensor *output = m_interpreter->tensor(t_outputs[0]);
    float* data= output->data.f;
    int height = output->dims->data[1];
    int width = output->dims->data[2];

    TRACE("size : ", height, " x ", width);

    std::shared_ptr<SelfieDescriptor> selfieDescriptor = std::dynamic_pointer_cast<SelfieDescriptor>(descriptor);

    // CID9333404, 9333392
    if (selfieDescriptor != nullptr) {
        selfieDescriptor->addMaskData(width, height, data);
    }

    return kAifOk;
}

} // end of namespace aif
