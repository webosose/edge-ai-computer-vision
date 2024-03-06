/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/pose/PosenetDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

namespace aif {

PosenetDetector::PosenetDetector(const std::string& modelPath)
    : Detector(modelPath)
    , m_iouThreshold(0.7f)
{
}

PosenetDetector::~PosenetDetector()
{
}

std::shared_ptr<DetectorParam> PosenetDetector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<PosenetParam>();
    return param;
}

t_aif_status PosenetDetector::fillInputTensor(const cv::Mat& img)/* override*/
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

    t_aif_status res;
    if (m_modelName.find("quant")!=std::string::npos) {
        res = aif::fillInputTensor<uint8_t, cv::Vec3b>(
                m_interpreter.get(),
                img,
                width,
                height,
                channels,
                true,
                aif::kAifNone
                );
    } else {
        res = aif::fillInputTensor<float, cv::Vec3f>(
                m_interpreter.get(),
                img,
                width,
                height,
                channels,
                false,
                aif::kAifStandardization
                );
    }
    if (res != kAifOk) {
        Loge("fillInputTensor failed!!");
        return kAifError;
    }

    return res;
}

t_aif_status PosenetDetector::preProcessing()
{
    return kAifOk;
}

void PosenetDetector::printOutput() {
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

t_aif_status PosenetDetector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    //printOutput();
    const std::vector<int> &t_outputs = m_interpreter->outputs();
    TfLiteTensor *output = m_interpreter->tensor(t_outputs[0]);         // poseCount * 17 keyPoints
    float* keyPoints = reinterpret_cast<float*>(output->data.data);
    size_t keyPointsSize = output->bytes/sizeof(float);

    TfLiteTensor *output2 = m_interpreter->tensor(t_outputs[1]);        // poseCount * 17 keyPoiints score
    float* keyPointsScore = reinterpret_cast<float*>(output2->data.data);

    TfLiteTensor *output3 = m_interpreter->tensor(t_outputs[2]);        // poseCount * pose score
    float* pose_score = reinterpret_cast<float*>(output3->data.data);

    TfLiteTensor *output4 = m_interpreter->tensor(t_outputs[3]);        // poseCount
    float* pose_count = output4->data.f;
    TRACE("poses count: ", *pose_count);

    std::shared_ptr<PosenetDescriptor> posenetDescriptor = std::dynamic_pointer_cast<PosenetDescriptor>(descriptor);
    size_t k = 0;
    for (size_t i = 0; i < *pose_count; i++ ) {
        std::vector<cv::Point2f> points;
        std::vector<float> scores;
        for (size_t j = 0; j < PosenetDescriptor::NUM_KEYPOINT_TYPES; j++ ) {
            if (keyPointsSize <= k) {
                Loge("keyPoints index error : ", k);
                return kAifError;
            }
            float height = keyPoints[k] / m_modelInfo.height;
            if (keyPointsSize <= k + 1) {
                Loge("keyPoints index error : ", k);
                return kAifError;
            }
            float width = keyPoints[CHECK_UINT_ADD(k,1)] / m_modelInfo.width;
            points.push_back(cv::Point2f(width, height));
            k = CHECK_UINT_ADD(k, 2);
            scores.push_back(keyPointsScore[i * PosenetDescriptor::NUM_KEYPOINT_TYPES + j]);
        }

        // CID9333396, CID9333362
        if (posenetDescriptor != nullptr) {
            posenetDescriptor->addKeyPoints(pose_score[i], points, scores);
        }
    }

    // CID9333396, CID9333362
    if (posenetDescriptor != nullptr) {
        m_prevPoses = posenetDescriptor->makeBodyParts(m_prevPoses, m_iouThreshold);
    }

    return kAifOk;
}

} // end of namespace aif
