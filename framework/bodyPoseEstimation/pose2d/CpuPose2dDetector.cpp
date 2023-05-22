/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/bodyPoseEstimation/pose2d/CpuPose2dDetector.h>
#include <aif/bodyPoseEstimation/pose2d/RegularPostProcess.h>
#include <aif/bodyPoseEstimation/pose2d/XtensorPostProcess.h>

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

        //cv::imwrite("./crop_input.jpg", img);
        cv::Mat inputImg;
        if (m_useUDP) {
            getAffinedImage(img, cv::Size(width, height), inputImg);
            //cv::imwrite("affined_input.jpg", inputImg);
        } else {
            getPaddedImage(img, cv::Size(width, height), inputImg);
        }
        //cv::imwrite("./pad_input.jpg", inputImg);

        //cv::cvtColor(inputImg, inputImg, cv::COLOR_BGR2RGB);
        inputImg.convertTo(inputImg, CV_32FC3);
        normalizeImage(inputImg);

        float* inputTensor = m_interpreter->typed_input_tensor<float>(0);
        if (inputTensor == nullptr) {
            throw std::runtime_error("inputTensor ptr is null");
            return kAifError;
        }
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
    Stopwatch sw;
    sw.start();
    const std::vector<int> &outputs = m_interpreter->outputs();
    TfLiteTensor *output = m_interpreter->tensor(outputs[0]);

    m_heatMapHeight = output->dims->data[1];
    m_heatMapWidth = output->dims->data[2];
    m_numKeyPoints = output->dims->data[3];

    int outputSize = m_heatMapWidth * m_heatMapHeight * m_numKeyPoints;

    float* buffer = new float[outputSize];
    memset(buffer, 0, sizeof(float) * outputSize);

    float* data= reinterpret_cast<float*>(output->data.data);
    int i = 0;
    for (int h = 0; h < m_heatMapHeight; h++) {
        for (int w = 0; w < m_heatMapWidth; w++) {
            for (int k = 0; k < m_numKeyPoints; k++) {
                int index = k * (m_heatMapWidth * m_heatMapHeight) + h * m_heatMapWidth + w;
                if ( index >= outputSize) continue;
                buffer[index] = data[i++];
            }
        }
    }

    std::shared_ptr<Pose2dDetector> detector = this->get_shared_ptr();
#if defined(USE_XTENSOR)
    m_postProcess= std::make_shared<XtensorPostProcess>(detector);
#else
    m_postProcess= std::make_shared<RegularPostProcess>(detector);
#endif
    if(!m_postProcess->execute(descriptor, buffer)){
        Loge("failed to get position x, y from heatmap");
        return kAifError;
    }

    delete [] buffer;

    TRACE("postProcessing(): ", sw.getMs(), "ms");
    sw.stop();

    return kAifOk;
}

void CpuPose2dDetector::normalizeImage(cv::Mat& img) const
{
    const float meanBGR[3] = { 0.406, 0.456, 0.485 };
    const float stdBGR[3] = { 4.44, 4.46, 4.36 };

    auto dataPtr = reinterpret_cast<float*>(img.data);
    auto total = img.total();

    for (auto i = 0; i < total; i++ ) {
        float* localPtr = dataPtr + i * 3;
        localPtr[0] = ( ( localPtr[0] * 0.003921569 ) - meanBGR[0] ) * stdBGR[0];
        localPtr[1] = ( ( localPtr[1] * 0.003921569 ) - meanBGR[1] ) * stdBGR[1];
        localPtr[2] = ( ( localPtr[2] * 0.003921569 ) - meanBGR[2] ) * stdBGR[2];
    }
}

} // namespace aif
