/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */
#include <aif/log/Logger.h>
#include <aif/bodyPoseEstimation/pose2d/CpuQuantPose2dDetector.h>
#include <aif/bodyPoseEstimation/pose2d/RegularPostProcess.h>
#include <aif/bodyPoseEstimation/pose2d/XtensorPostProcess.h>

#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

CpuQuantPose2dDetector::CpuQuantPose2dDetector()
    : Pose2dDetector("pose2d_mobilenet_full_integer_quant.tflite")
{
}

CpuQuantPose2dDetector::~CpuQuantPose2dDetector()
{
}

void CpuQuantPose2dDetector::setModelInfo(TfLiteTensor* inputTensor)
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

t_aif_status CpuQuantPose2dDetector::fillInputTensor(const cv::Mat& img)/* override*/
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

        cv::Mat inputImg;
        if (m_useUDP) {
            getAffinedImage(img, cv::Size(width, height), inputImg);
            //cv::imwrite("affined_input_cpuquant.jpg", inputImg);
        } else {
            getPaddedImage(img, cv::Size(width, height), inputImg);
        }
        inputImg.convertTo(inputImg, CV_8UC3);

        int8_t* inputTensor = m_interpreter->typed_input_tensor<int8_t>(0);
        if (inputTensor == nullptr) {
            throw std::runtime_error("inputTensor ptr is null");
            return kAifError;
        }
        std::memcpy(inputTensor, inputImg.ptr<int8_t>(0), width * height * channels * sizeof(int8_t));

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


t_aif_status CpuQuantPose2dDetector::preProcessing()
{
   return kAifOk;
}

t_aif_status CpuQuantPose2dDetector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    const std::vector<int> &outputs = m_interpreter->outputs();
    TfLiteTensor *output = m_interpreter->tensor(outputs[0]);
    if (output == nullptr) {
        throw std::runtime_error("can't get tflite tensor_output!!");
    }
    TfLiteAffineQuantization* q_params = reinterpret_cast<TfLiteAffineQuantization*>(output->quantization.params);
    if (!q_params) {
        throw std::runtime_error("output tensor doesn't have q_params...");
    }

    if (q_params->scale->size != 1) {
        throw std::runtime_error("output tensor should not per-axis quant...");
    }
    float scale = q_params->scale->data[0];
    int zeroPoint= q_params->zero_point->data[0];
    Logi("scale: ", scale, " zero_point: ", zeroPoint);

    m_heatMapHeight = output->dims->data[1];
    m_heatMapWidth = output->dims->data[2];
    m_numKeyPoints = output->dims->data[3];

    int outputSize = m_heatMapWidth * m_heatMapHeight * m_numKeyPoints;
    float* buffer = new float[outputSize];
    memset(buffer, 0, sizeof(float) * outputSize);

    int8_t* data= reinterpret_cast<int8_t*>(output->data.data);
    int i = 0;
    for (int h = 0; h < m_heatMapHeight; h++) {
        for (int w = 0; w < m_heatMapWidth; w++) {
            for (int k = 0; k < m_numKeyPoints; k++) {
                int index = k * (m_heatMapWidth * m_heatMapHeight) + h * m_heatMapWidth + w;
                if ( index >= outputSize) continue;
                buffer[index] = scale * (static_cast<int>(data[i++]) - zeroPoint);
            }
        }
    }

    std::shared_ptr<Pose2dDetector> detector;
    try {
        detector = this->get_shared_ptr();
    } catch(const std::bad_weak_ptr& bwp) {
        Loge("failed to get CpuQuantPose2dDetector's shared_ptr. It got bad_weak_ptr!");
        delete [] buffer;
        return kAifError;
    }
    m_postProcess = std::make_shared<RegularPostProcess>(detector);
    if(!m_postProcess->execute(descriptor, buffer)){
        Loge("failed to get position x, y from heatmap");
        delete [] buffer;
        return kAifError;
    }

    delete [] buffer;
    return kAifOk;
}

} // namespace aif
