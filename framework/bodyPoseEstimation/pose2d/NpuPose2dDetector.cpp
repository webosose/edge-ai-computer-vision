/*
 * Copyright (c) 2023 LG Electronics Inc.
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

NpuPose2dDetector::NpuPose2dDetector(const std::string& modelName)
    : mScaleIn(0.0f)
    , mZeropointIn(0)
    , Pose2dDetector(modelName)
{
}

NpuPose2dDetector::~NpuPose2dDetector()
{
}

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

        cv::Mat inputImg, inputNormImg;
        if (m_useUDP) {
            getAffinedImage(img, cv::Size(width, height), inputImg);
            /* cv::imwrite("affined_input_npu.jpg", inputImg); // for debugging!!  */
            //memoryDump(inputImg.data, "./orig_input.bin", width * height * channels * sizeof(uint8_t));
        } else {
            inputImg = img;
        }
        //cv::imwrite("./padded_npu.jpg", inputImg);
        //cv::cvtColor(inputImg, inputImg, cv::COLOR_BGR2RGB);

        inputNormImg = inputImg;
        inputImg.convertTo(inputImg, CV_32FC3);
        inputNormImg.convertTo(inputNormImg, CV_8UC3);

        normalizeImageWithQuant(inputImg, inputNormImg);
        //memoryDump(inputNormImg.data, "./normQuant_input.bin", width * height * channels * sizeof(uint8_t));
        //cv::imwrite("./normQuant_input.jpg", inputNormImg);

        uint8_t* inputTensor = m_interpreter->typed_input_tensor<uint8_t>(0);
        if (inputTensor == nullptr) {
            throw std::runtime_error("inputTensor ptr is null");
            return kAifError;
        }
        std::memcpy(inputTensor, inputNormImg.ptr<uint8_t>(0), width * height * channels * sizeof(uint8_t));
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
    const std::vector<int> &inputs = m_interpreter->inputs();
    TfLiteTensor *input = m_interpreter->tensor(inputs[0]);
    getInputTensorInfo(input);

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
    //memoryDump(data, "./output.bin", m_numKeyPoints * m_heatMapHeight * m_heatMapWidth * sizeof(float));
    //memoryRestore(data, "/usr/share/aif/example/m5_6_before_post_processing.bin");

    std::shared_ptr<Pose2dDetector> detector = this->get_shared_ptr();

    m_postProcess= std::make_shared<XtensorPostProcess>(detector);

    if(!m_postProcess->execute(descriptor, data)){
        Loge("failed to get position x, y from heatmap");
        return kAifError;
    }

    TRACE("postProcessing(): ", sw.getMs(), "ms");
    sw.stop();

    return kAifOk;
}

void NpuPose2dDetector::getInputTensorInfo(TfLiteTensor *input)
{
    if (input == nullptr || input->dims == nullptr) {
        throw std::runtime_error("input / input->dims is nullptr");
    }

    TfLiteAffineQuantization* q_params = reinterpret_cast<TfLiteAffineQuantization*>(input->quantization.params);
    if (!q_params) {
        throw std::runtime_error("input tensor doesn't have q_params...");
    }

    if (q_params->scale->size != 1) {
        throw std::runtime_error("input tensor should not per-axis quant...");
    }

    mScaleIn = q_params->scale->data[0];
    mZeropointIn = q_params->zero_point->data[0];

    TRACE( __func__, "[POSE2D!!!!!!!!!!]  mScaleIn: " , mScaleIn , " mZeropointIn: " , mZeropointIn);
}

void NpuPose2dDetector::normalizeImageWithQuant(cv::Mat& img, cv::Mat& normImg) const
{
    auto QUANT = [this](float data) {
        return static_cast<uint8_t> (std::max(std::min(((data / mScaleIn) + mZeropointIn), 255.f), 0.0f)); // fixed
    };

    const float meanBGR[3] = { 0.406, 0.456, 0.485 }; // B,G,R : R is biggest!!!
    const float stdBGR[3] = { 4.44, 4.46, 4.36 };     // B,G,R

    auto dataPtr = reinterpret_cast<float*>(img.data);
    auto norm_dataPtr = reinterpret_cast<uint8_t*>(normImg.data);
    auto total = img.total();

    // Assume img is BGR, normImg is BGR
    for (auto i = 0; i < total; i++ ) {
        float* localPtr = dataPtr + i * 3;
        uint8_t* norm_localPtr = norm_dataPtr + i * 3;
#if 1 // for debugging
        localPtr[0] = ( ( localPtr[0] * 0.003921569 ) - meanBGR[0] ) * stdBGR[0]; // B
        localPtr[1] = ( ( localPtr[1] * 0.003921569 ) - meanBGR[1] ) * stdBGR[1]; // G
        localPtr[2] = ( ( localPtr[2] * 0.003921569 ) - meanBGR[2] ) * stdBGR[2]; // R
        norm_localPtr[0] = QUANT(localPtr[0]);
        norm_localPtr[1] = QUANT(localPtr[1]);
        norm_localPtr[2] = QUANT(localPtr[2]);
#else
        norm_localPtr[0] = QUANT(( ( localPtr[0] * 0.003921569 ) - meanBGR[0] ) * stdBGR[0]);
        norm_localPtr[1] = QUANT(( ( localPtr[1] * 0.003921569 ) - meanBGR[1] ) * stdBGR[1]);
        norm_localPtr[2] = QUANT(( ( localPtr[2] * 0.003921569 ) - meanBGR[2] ) * stdBGR[2]);
#endif
    }

}

} // namespace aif
