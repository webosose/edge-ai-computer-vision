/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/poseLandmark/PoseLandmarkDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

namespace aif {

PoseLandmarkDetector::PoseLandmarkDetector(const std::string& modelPath)
    : Detector(modelPath )
    , m_iouThreshold(0.7f)
{
}

PoseLandmarkDetector::~PoseLandmarkDetector()
{
}

std::shared_ptr<DetectorParam> PoseLandmarkDetector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<PoseLandmarkParam>();
    return param;
}

t_aif_status PoseLandmarkDetector::fillInputTensor(const cv::Mat& img)/* override*/
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

t_aif_status PoseLandmarkDetector::preProcessing()
{
    return kAifOk;
}

void PoseLandmarkDetector::printOutput() {
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

t_aif_status PoseLandmarkDetector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    //printOutput();
    std::shared_ptr<PoseLandmarkDescriptor> poseLandmarkDescriptor = std::dynamic_pointer_cast<PoseLandmarkDescriptor>(descriptor);

    const std::vector<int> &outputs = m_interpreter->outputs();
    TfLiteTensor *landmarks = m_interpreter->tensor(outputs[0]);
    TfLiteTensor *segments = m_interpreter->tensor(outputs[2]);
    int height = segments->dims->data[1];
    int width = segments->dims->data[2];
    TRACE("size : ", height, " x ", width);

    // CID9333390, CID9333401
    if (poseLandmarkDescriptor != nullptr) {
        poseLandmarkDescriptor->addMaskData(width, height, segments->data.f);
    }

    std::vector<std::vector<float>> outLandmarks(PoseLandmarkDescriptor::NUM_LANDMARK_TYPES);
    for (int i = 0; i < PoseLandmarkDescriptor::NUM_LANDMARK_TYPES; i++) {
        for (int j = 0; j < PoseLandmarkDescriptor::NUM_LANDMARK_ITEMS; j++) {
            size_t index = i * PoseLandmarkDescriptor::NUM_LANDMARK_ITEMS + j;
            float data = landmarks->data.f[index];
            if (j == PoseLandmarkDescriptor::COOD_X) {
                data = data / 255.0f;
            } else if (j == PoseLandmarkDescriptor::COOD_Y) {
                data = data / 255.0f;
            } else if (j == PoseLandmarkDescriptor::VISIBILITY ||
                       j == PoseLandmarkDescriptor::PRESENCE) {
                data = sigmoid<float>(data);    // sigmoid
            }
            outLandmarks[i].emplace_back(data);
        }
    }

    // CID333390
    if (poseLandmarkDescriptor != nullptr) {
        poseLandmarkDescriptor->addLandmarks(outLandmarks);
        m_prevPoses = poseLandmarkDescriptor->makeBodyParts(m_prevPoses, m_iouThreshold);
    }

    return kAifOk;
}

} // end of namespace aif
