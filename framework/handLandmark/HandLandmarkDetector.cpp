/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/handLandmark/HandLandmarkDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

namespace aif {

HandLandmarkDetector::HandLandmarkDetector(const std::string& modelPath)
    : Detector(modelPath)
{
}

HandLandmarkDetector::~HandLandmarkDetector()
{
}

std::shared_ptr<DetectorParam> HandLandmarkDetector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<HandLandmarkParam>();
    return param;
}

t_aif_status HandLandmarkDetector::fillInputTensor(const cv::Mat& img)/* override*/
{
    try {
        if (img.rows == 0 || img.cols == 0) {
            throw std::runtime_error("invalid opencv image!!");
        }

        int height = m_modelInfo.height;
        int width = m_modelInfo.width;
        int channels = m_modelInfo.channels;

        if (m_interpreter == nullptr) {
            throw std::runtime_error("hand_landmark_lite.tflite interpreter not initialized!!");
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
            throw std::runtime_error("fillInputTensor failed!!");
        }

        return res;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
}

t_aif_status HandLandmarkDetector::preProcessing()
{
    return kAifOk;
}

void HandLandmarkDetector::printOutput() {
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

t_aif_status HandLandmarkDetector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    std::shared_ptr<HandLandmarkDescriptor> handLandmarkDescriptor =
        std::dynamic_pointer_cast<HandLandmarkDescriptor>(descriptor);

    const std::vector<int> &outputs = m_interpreter->outputs();
    TfLiteTensor *landmarks = m_interpreter->tensor(outputs[0]);
    TfLiteTensor *presence = m_interpreter->tensor(outputs[1]);
    TfLiteTensor *handedness = m_interpreter->tensor(outputs[2]);
    TfLiteTensor *landmarks3d = m_interpreter->tensor(outputs[3]);

    // CID9333377, CID9333397
    if (handLandmarkDescriptor != nullptr) {
        handLandmarkDescriptor->addLandmarks(
             *presence->data.f, *handedness->data.f, landmarks->data.f, landmarks3d->data.f);
    }

    return kAifOk;
}

} // end of namespace aif
