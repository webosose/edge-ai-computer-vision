/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/movenet/MovenetDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

namespace aif {

MovenetDetector::MovenetDetector(const std::string& modelPath)
    : Detector(modelPath)
{
}

MovenetDetector::~MovenetDetector()
{
}

std::shared_ptr<DetectorParam> MovenetDetector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<MovenetParam>();
    return param;
}

t_aif_status MovenetDetector::fillInputTensor(const cv::Mat& img)/* override*/
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
        Loge("fillInputTensor failed!!");
        return kAifError;
    }

    return res;
}

t_aif_status MovenetDetector::preProcessing()
{
    return kAifOk;
}

void MovenetDetector::printOutput() {
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

t_aif_status MovenetDetector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    const std::vector<int> &outputs = m_interpreter->outputs();
    TfLiteTensor *output = m_interpreter->tensor(outputs[0]);
    float* keyPoints = reinterpret_cast<float*>(output->data.data);
    size_t keyPointsSize = output->bytes / sizeof(float);

    std::shared_ptr<MovenetDescriptor> movenetDescriptor = std::dynamic_pointer_cast<MovenetDescriptor>(descriptor);
    float scaleX = (float)img.size().width / (float)m_modelInfo.width;
    float scaleY = (float)img.size().height / (float)m_modelInfo.height;

    TRACE("movenet img size: ", img.size());
    TRACE("movenet model size: ", m_modelInfo.width, m_modelInfo.height);
    TRACE("scale x: ", scaleX);
    TRACE("scale y: ", scaleY);

    size_t k = 0;
    std::vector<cv::Point2f> points;
    std::vector<float> scores;
    for (size_t j = 0; j < 17; j++ ) {
        if (keyPointsSize <= k) {
            Loge("keyPoints index error : ", k);
            return kAifError;
        }
        float height = keyPoints[k]; // * scaleY;

        if (keyPointsSize <= k + 1) {
            Loge("keyPoints index error : ", k);
            return kAifError;
        }
        float width = keyPoints[k+1];// * scaleX;
        points.push_back(cv::Point2f(width, height));

        if (keyPointsSize <= k + 2) {
            Loge("keyPoints index error : ", k);
            return kAifError;
        }
        scores.push_back(keyPoints[k+2]);
        k = k + 3;
    }

    // CID9333407, CID9333379
    if (movenetDescriptor != nullptr) {
        movenetDescriptor->addKeyPoints(points, scores);
    }
    return kAifOk;
}

} // end of namespace aif
