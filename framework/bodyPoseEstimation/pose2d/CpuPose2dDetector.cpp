/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/bodyPoseEstimation/pose2d/CpuPose2dDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

CpuPose2dDetector::CpuPose2dDetector()
    : Pose2dDetector("pose2d_mobilenet_float32.tflite") {}

CpuPose2dDetector::~CpuPose2dDetector() {}


void CpuPose2dDetector::setModelInfo(TfLiteTensor* inputTensor)
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

t_aif_status CpuPose2dDetector::fillInputTensor(const cv::Mat& img)/* override*/
{
    try {
        if (img.rows == 0 || img.cols == 0) {
            throw std::runtime_error("invalid opencv image!!");
        }

        if (m_interpreter == nullptr) {
            throw std::runtime_error("pose2d.tflite interpreter not initialized!!");
        }

        int height = m_modelInfo.height;
        int width = m_modelInfo.width;
        int channels = m_modelInfo.channels;

        //cv::imwrite("/media/result/input.jpg", img);
        cv::Mat inputImg;
        getPaddedImage(img, cv::Size(width, height), inputImg);

        //cv::imwrite("/media/result/padded_cpu.jpg", inputImg);
        cv::cvtColor(inputImg, inputImg, cv::COLOR_BGR2RGB);
        inputImg.convertTo(inputImg, CV_32FC3);
        inputImg /= 255.0f;

        float* inputTensor = m_interpreter->typed_input_tensor<float>(0);
        std::memcpy(inputTensor, inputImg.ptr<float>(0), width * height * channels * sizeof(float));
        return kAifOk;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }
    return kAifOk;
}


t_aif_status CpuPose2dDetector::preProcessing()
{
   return kAifOk;
}

t_aif_status CpuPose2dDetector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    const std::vector<int> &outputs = m_interpreter->outputs();
    TfLiteTensor *output = m_interpreter->tensor(outputs[0]);

    m_heatMapHeight = output->dims->data[1];
    m_heatMapWidth = output->dims->data[2];
    m_numKeyPoints = output->dims->data[3];

    int outputSize = m_heatMapWidth * m_heatMapHeight * m_numKeyPoints;

    float* buffer = new float[outputSize];
    memset(buffer, 0, sizeof(buffer));

    float* data= reinterpret_cast<float*>(output->data.data);
    int i = 0;
    for (int h = 0; h < m_heatMapHeight; h++) {
        for (int w = 0; w < m_heatMapWidth; w++) {
            for (int k = 0; k < m_numKeyPoints; k++) {
                buffer[k * (m_heatMapWidth * m_heatMapHeight) + h * m_heatMapWidth + w] = data[i++];
            }
        }
    }

    if (!processHeatMap(img, descriptor, buffer)) {
        Loge("failed to get position x, y from heatmap");
        return kAifError;
    }

    delete [] buffer;
    return kAifOk;
}

} // namespace aif
