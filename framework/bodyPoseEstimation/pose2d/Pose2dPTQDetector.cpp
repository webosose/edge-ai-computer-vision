/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/bodyPoseEstimation/pose2d/XtensorPostProcess.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dPTQDetector.h>

#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

Pose2dPTQDetector::Pose2dPTQDetector(const std::string& modelName, std::type_index ioType)
    : mScaleIn(0.0f)
    , mZeropointIn(0)
    , mScaleOut(0.0f)
    , mZeropointOut(0)
    , output_heatmaps(nullptr)
    , mIoType(ioType)
    , Pose2dDetector(modelName)
{
}

Pose2dPTQDetector::~Pose2dPTQDetector()
{
}

void Pose2dPTQDetector::setModelInOutInfo(const std::vector<int> &t_inputs, const std::vector<int> &t_outputs)
{
    if (t_inputs.size() != 1) {
        Loge("input tensor size should be 1 ");
        return ;
    }

    TfLiteTensor *inputTensor = m_interpreter->tensor(t_inputs[0]);
    if (inputTensor == nullptr || inputTensor->dims == nullptr) {
        Loge("tflite inputTensor invalid!!");
        return;
    }

    if (inputTensor->dims == nullptr) {
        Loge("invalid inputTensor dimension!");
        return;
    }

    /* Input Tensor Info */
    m_modelInfo.inputSize = inputTensor->dims->size;
    m_modelInfo.batchSize = 1;
    m_modelInfo.height = inputTensor->dims->data[1];
    m_modelInfo.width = inputTensor->dims->data[2];
    m_modelInfo.channels = inputTensor->dims->data[3];

    TRACE("input_size: ", m_modelInfo.inputSize);
    TRACE("batch_size: ", m_modelInfo.batchSize);
    TRACE("height:"     , m_modelInfo.height);
    TRACE("width: "     , m_modelInfo.width);
    TRACE("channels: "  , m_modelInfo.channels);

    auto quant = getQuantizationTensorInfo(inputTensor);
    mScaleIn = quant.first;
    mZeropointIn = quant.second;

    TRACE("input  mScaleIn: " , mScaleIn , " mZeropointIn: " , mZeropointIn);

    if (t_outputs.size() != 1) {
        Loge("output tensor size should be 1!");
        return ;
    }

    /* Output Tensor Info */
    output_heatmaps = m_interpreter->tensor(t_outputs[0]);

    m_numKeyPoints = output_heatmaps->dims->data[3]; // 41
    m_heatMapHeight = output_heatmaps->dims->data[1]; // 64
    m_heatMapWidth = output_heatmaps->dims->data[2]; // 48

    TRACE("numKeyPoints: ", m_numKeyPoints);
    TRACE("heatMapHeight: ", m_heatMapHeight);
    TRACE("heatMapWidth: ", m_heatMapWidth);

    auto quant_o = getQuantizationTensorInfo(output_heatmaps);
    mScaleOut = quant_o.first;
    mZeropointOut = quant_o.second;

    TRACE("output  mScaleOut: " , mScaleOut , " mZeropointOut: " , mZeropointOut);
}

t_aif_status Pose2dPTQDetector::fillInputTensor(const cv::Mat& img)/* override*/
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
            //cv::imwrite("affined_input_npu.jpg", inputImg);
            //memoryDump(inputImg.data, "./orig_input.bin", width * height * channels * sizeof(uint8_t));
        } else {
            inputImg = img;
        }
        //cv::imwrite("./padded_npu.jpg", inputImg);
        //cv::cvtColor(inputImg, inputImg, cv::COLOR_BGR2RGB);

        inputNormImg = inputImg;
        inputImg.convertTo(inputImg, CV_32FC3);

        if (mIoType == typeid(uint8_t)) {
            inputNormImg.convertTo(inputNormImg, CV_8UC3);
            normalizeImageWithQuant<uint8_t>(inputImg, inputNormImg);
            //memoryDump(inputNormImg.data, "./normQuant_input.bin", width * height * channels * sizeof(uint8_t));
            //cv::imwrite("./normQuant_input.jpg", inputNormImg);

            copyInputTensor<uint8_t>(inputNormImg, width * height * channels);
        } else if (mIoType == typeid(int8_t)) {
            inputNormImg.convertTo(inputNormImg, CV_8SC3);
            normalizeImageWithQuant<int8_t>(inputImg, inputNormImg);

            copyInputTensor<int8_t>(inputNormImg, width * height * channels);
        } else {
            throw std::runtime_error("wrong io Type");
        }

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

t_aif_status Pose2dPTQDetector::preProcessing()
{
    return kAifOk;
}

t_aif_status Pose2dPTQDetector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    Stopwatch sw;
    sw.start();

    int outputSize = m_heatMapWidth * m_heatMapHeight * m_numKeyPoints;
    float* buffer = new float[outputSize];
    memset(buffer, 0, sizeof(float) * outputSize);

    try {
        if (mIoType == typeid(uint8_t)) {
            copyOutputTensor<uint8_t>(buffer, outputSize);
        } else if (mIoType == typeid(int8_t)) {
            copyOutputTensor<int8_t>(buffer, outputSize);
        } else {
            throw std::runtime_error(" mIoType should be uint8_t or int8_t!!");
        }

        std::shared_ptr<Pose2dDetector> detector;
        detector = this->get_shared_ptr();
        m_postProcess= std::make_shared<XtensorPostProcess>(detector);

        if(!m_postProcess->execute(descriptor, buffer)){
            throw std::runtime_error(" failed to get position x, y from heatmap");
        }
    } catch(const std::bad_weak_ptr& bwp) {
        Loge("failed to get Pose2dPTQDetector's shared_ptr. It got bad_weak_ptr!");
        delete [] buffer;
        sw.stop();
        return kAifError;
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        delete [] buffer;
        sw.stop();
        return kAifError;
    }

    delete [] buffer;

    TRACE("postProcessing(): ", sw.getMs(), "ms");
    sw.stop();

    return kAifOk;
}

template <typename T>
t_aif_status Pose2dPTQDetector::copyInputTensor(cv::Mat& inputNormImg, size_t size_whc)
{
    T* inputTensor = m_interpreter->typed_input_tensor<T>(0);
    if (inputTensor == nullptr) {
        Loge(__func__, " inputTensor ptr is null");
        return kAifError;
    }
    std::memcpy(inputTensor, inputNormImg.ptr<T>(0), size_whc * sizeof(T));
    //memoryDump(inputTensor, "./input.bin", width * height * channels * sizeof(uint8_t));
    return kAifOk;
}

template <typename T>
t_aif_status Pose2dPTQDetector::copyOutputTensor(float *buffer, size_t outputSize)
{
    T* data= reinterpret_cast<T*>(output_heatmaps->data.data);
    int i = 0;
    for (int h = 0; h < m_heatMapHeight; h++) {
        for (int w = 0; w < m_heatMapWidth; w++) {
            for (int k = 0; k < m_numKeyPoints; k++) {
                int index = k * (m_heatMapWidth * m_heatMapHeight) + h * m_heatMapWidth + w;
                if ( index >= outputSize) continue;
                buffer[index] = mScaleOut * (data[i++] - mZeropointOut);
            }
        }
    }

    return kAifOk;
}

template <typename T>
void Pose2dPTQDetector::normalizeImageWithQuant(cv::Mat& img, cv::Mat& normImg) const
{
    float upper, lower = 0.0f;
    if (typeid(T) == typeid(uint8_t)) {
        upper = 255.f; lower = 0.0f;
    } else if (typeid(T) == typeid(int8_t)) {
        upper = 127.f; lower = -128.f;
    } else {
        Loge(__func__, " type is wrong!!");
        return;
    }

    auto QUANT = [=](float data) {
        return static_cast<T> (std::max(std::min(((data / mScaleIn) + mZeropointIn), upper), lower));
    };

    const float meanBGR[3] = { 0.406, 0.456, 0.485 }; // B,G,R : R is biggest!!!
    const float stdBGR[3] = { 4.44, 4.46, 4.36 };     // B,G,R

    auto dataPtr = reinterpret_cast<float*>(img.data);
    auto norm_dataPtr = reinterpret_cast<T*>(normImg.data);
    auto total = img.total();

    // Assume img is BGR, normImg is BGR
    for (auto i = 0; i < total; i++ ) {
        float* localPtr = dataPtr + i * 3;
        T* norm_localPtr = norm_dataPtr + i * 3;
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


} // end of namespace aif


