/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/bodyPoseEstimation/pose2d/NpuPose2dDetector.h>
#include <aif/bodyPoseEstimation/pose2d/RegularPostProcess.h>
#include <aif/bodyPoseEstimation/pose2d/XtensorPostProcess.h>

#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

NpuPose2dDetector::NpuPose2dDetector()
    : Pose2dDetector("FitTV_Pose2D_Division.tflite") {}

NpuPose2dDetector::~NpuPose2dDetector() {}

void NpuPose2dDetector::setModelInfo(TfLiteTensor* inputTensor)
{
    m_modelInfo.batchSize = 1;
    m_modelInfo.height = inputTensor->dims->data[0];
    m_modelInfo.width = inputTensor->dims->data[1];
    m_modelInfo.channels = inputTensor->dims->data[2];

    TRACE("input_size: ", m_modelInfo.inputSize);
    TRACE("batch_size: ", m_modelInfo.batchSize);
    TRACE("height:", m_modelInfo.height);
    TRACE("width: ", m_modelInfo.width);
    TRACE("channels: ", m_modelInfo.channels);
}

t_aif_status NpuPose2dDetector::fillInputTensor(const cv::Mat& img)/* override*/
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
            //cv::imwrite("affined_input_npu.jpg", inputImg);
        } else {
            getPaddedImage(img, cv::Size(width, height), inputImg);
        }

        //cv::imwrite("./padded_npu.jpg", inputImg);
        //cv::cvtColor(inputImg, inputImg, cv::COLOR_BGR2RGB);
        inputImg.convertTo(inputImg, CV_8UC3);
        uint8_t* inputTensor = m_interpreter->typed_input_tensor<uint8_t>(0);
        std::memcpy(inputTensor, inputImg.ptr<uint8_t>(0), width * height * channels * sizeof(uint8_t));

        //memoryDump(inputTensor, "./input.bin", width * height * channels * sizeof(uint8_t));
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

t_aif_status NpuPose2dDetector::preProcessing()
{
    return kAifOk;
}

t_aif_status NpuPose2dDetector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    Stopwatch sw;
    sw.start();

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

    m_numKeyPoints = output->dims->data[1];
    m_heatMapHeight = output->dims->data[2];
    m_heatMapWidth = output->dims->data[3];

    float* data= reinterpret_cast<float*>(output->data.data);

    std::shared_ptr<Pose2dDetector> detector = this->get_shared_ptr();
#if defined(USE_XTENSOR)
    m_postProcess= std::make_shared<XtensorPostProcess>(detector);
#else
    m_postProcess= std::make_shared<RegularPostProcess>(detector);
#endif
    if(!m_postProcess->execute(descriptor, data)){
        Loge("failed to get position x, y from heatmap");
        return kAifError;
    }

    TRACE("postProcessing(): ", sw.getMs(), "ms");
    sw.stop();

    return kAifOk;
}

} // namespace aif
