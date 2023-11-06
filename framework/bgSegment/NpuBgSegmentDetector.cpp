/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/bgSegment/NpuBgSegmentDetector.h>
#include <aif/bgSegment/BgSegmentDescriptor.h>
#include <aif/bgSegment/BgSegmentParam.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

NpuBgSegmentDetector::NpuBgSegmentDetector()
    : m_paddingInfo{0,}
    , m_useRoi(false)
    , m_outScaleUp(true) // default : AI Framework scale up the out image.
    , BgSegmentDetector("O24_SIC_SEG_v1.0_230906.tflite") {} // TODO: change the name, and initialize....!!!

NpuBgSegmentDetector::~NpuBgSegmentDetector() {}

void NpuBgSegmentDetector::setModelInfo(TfLiteTensor* inputTensor)
{
    if (inputTensor == nullptr) {
        Loge("inputTensor ptr is null");
        return;
    }

    m_modelInfo.inputSize = inputTensor->dims->size;   // inputSize
    m_modelInfo.batchSize = 1;
    m_modelInfo.height = inputTensor->dims->data[0]; // 270
    m_modelInfo.width = inputTensor->dims->data[1];  // 480
    m_modelInfo.channels = inputTensor->dims->data[2]; // 3

    TRACE("input_size: ", m_modelInfo.inputSize);
    TRACE("batch_size: ", m_modelInfo.batchSize);
    TRACE("height:", m_modelInfo.height);
    TRACE("width: ", m_modelInfo.width);
    TRACE("channels: ", m_modelInfo.channels);
}

t_aif_status NpuBgSegmentDetector::fillInputTensor(const cv::Mat& img)/* override*/
{
    try {
        if (img.rows == 0 || img.cols == 0) {
              throw std::runtime_error("invalid opencv image!!");
        }

        int height = m_modelInfo.height;
        int width = m_modelInfo.width;
        int channels = m_modelInfo.channels;

        if (m_interpreter == nullptr) {
            throw std::runtime_error("yolov3_v3.tflite interpreter not initialized!!");
        }

        cv::Mat img_resized;
        if ( isRoiValid(img.cols, img.rows, m_roiRect) ) {
            m_useRoi = true;
            cv::Mat roi_img = img( m_roiRect );
            m_paddingInfo = getPaddedImage(roi_img, cv::Size(width, height), img_resized);
        } else {
            m_useRoi = false;
            m_paddingInfo = getPaddedImage(img, cv::Size(width, height), img_resized);
        }

        //TRACE("resized size: ", img_resized.size());
        if (img_resized.rows != height || img_resized.cols != width) {
            throw std::runtime_error("image resize failed!!");
        }

        uint8_t* inputTensor = m_interpreter->typed_input_tensor<uint8_t>(0);
        std::memcpy(inputTensor, img_resized.ptr<uint8_t>(0), width * height * channels * sizeof(uint8_t));

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

t_aif_status NpuBgSegmentDetector::preProcessing()
{
    try {
        std::shared_ptr<BgSegmentParam> param = std::dynamic_pointer_cast<BgSegmentParam>(m_param);
        if (param == nullptr) {
            throw std::runtime_error("failed to convert DetectorParam to BgSegmentParam");
        }

        m_outScaleUp = param->outScaleUp;
        m_roiRect = {param->origImgRoiX, param->origImgRoiY, param->origImgRoiWidth, param->origImgRoiHeight};

        //TRACE(" m_roiRect isss ", m_roiRect.x, " ", m_roiRect.y, " ", m_roiRect.width, " ", m_roiRect.height);
        return kAifOk;
    } catch (const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }

    return kAifOk;
}

t_aif_status NpuBgSegmentDetector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    const std::vector<int> &t_outputs = m_interpreter->outputs();
    TfLiteTensor *output = m_interpreter->tensor(t_outputs[0]);
    int height = output->dims->data[0];
    int width = output->dims->data[1];

    //TRACE("size : ", height, " x ", width);

    std::shared_ptr<BgSegmentDescriptor> bgSegmentDescriptor = std::dynamic_pointer_cast<BgSegmentDescriptor>(descriptor);

    std::pair<int, int> maskSize;
    if (m_outScaleUp) {
        maskSize = scaleUpMask(width, height, output->data.uint8, img);
    } else {
        maskSize = getMask(width, height, output->data.uint8);
    }

    if (bgSegmentDescriptor != nullptr) {
        if (m_useRoi) {
            bgSegmentDescriptor->addMaskInfo(m_roiRect.x, m_roiRect.y, m_roiRect.width, m_roiRect.height,
                                             maskSize.first, maskSize.second);
        } else {
            bgSegmentDescriptor->addMaskInfo(0, 0, img.cols, img.rows,
                                             maskSize.first, maskSize.second);
        }
        //TRACE(__func__, " m_scaledUpMask size is : ", m_scaledUpMask.total() * m_scaledUpMask.elemSize());
        bgSegmentDescriptor->addExtraOutput( ExtraOutputType::UINT8_ARRAY, static_cast<void*>(m_scaledUpMask.data),
                                             m_scaledUpMask.total() * m_scaledUpMask.elemSize());
    }

    return kAifOk;
}

std::pair<int, int>
NpuBgSegmentDetector::scaleUpMask(int width, int height, uint8_t* srcData, const cv::Mat &origImg)
{
    // temporary
    // 1. => 135 x 240. remove SIC row padding
    cv::Mat padOffData(135, width, CV_8UC1, srcData);
    //cv::imwrite("./padOffData.jpg", padOffData);

    // 2. => 270 x 480. x 4 scaled up to model input
    cv::Mat resizedData;
    cv::resize(padOffData, resizedData, cv::Size(m_modelInfo.width, m_modelInfo.height), 0, 0, cv::INTER_LINEAR);
    //cv::imwrite("./resizedToInput.jpg", resizedData);

    // 3. => (h x 480 or 270 x w). remove padding
    cv::Rect bound(m_paddingInfo.leftBorder, m_paddingInfo.topBorder,
                   m_modelInfo.width - (m_paddingInfo.rightBorder + m_paddingInfo.leftBorder), // w
                   m_modelInfo.height - (m_paddingInfo.bottomBorder - m_paddingInfo.topBorder)); // h
    cv::Mat results = resizedData(bound);
    //cv::imwrite("./results.jpg", results);

    std::pair<int, int> maskSize;
    if (m_useRoi) {
    // 4. => roi rect. scale up to roi size.
    // TODO: ?? release memory ??
        cv::resize(results, m_scaledUpMask, cv::Size(m_roiRect.width, m_roiRect.height), 0, 0, cv::INTER_LINEAR);
        maskSize = std::make_pair(m_roiRect.width, m_roiRect.height);
    } else {
    // 4. => original img rect. scale up to original img size.
        cv::resize(results, m_scaledUpMask, cv::Size(origImg.cols, origImg.rows), 0, 0, cv::INTER_LINEAR);
        maskSize = std::make_pair(origImg.cols, origImg.rows);
    }
    //cv::imwrite("./m_scaledUpMask.jpg", m_scaledUpMask);

    // 5. flatten it.
    m_scaledUpMask = m_scaledUpMask.reshape(1, 1);

    return maskSize;
}

std::pair<int, int> NpuBgSegmentDetector::getMask(int width, int height, uint8_t* srcData)
{
    // temporary
    // 1. => 135 x 240. remove SIC row padding
    cv::Mat padOffData(135, width, CV_8UC1, srcData);
    //cv::imwrite("./padOffData.jpg", padOffData);

    float scaleW = static_cast<float>(width) / m_modelInfo.width; // out/in ratio 135/270 = 0.5
    float scaleH = 135.0 / m_modelInfo.height; // out/in ratio 240/480 = 0.5
    //TRACE(__func__, " scaleW: ", scaleW, " scaleH: " , scaleH);

    // 2. => l/r/t/b padding off in 135 x 240. remove SWP padding.
    cv::Rect bound(m_paddingInfo.leftBorder*scaleW, m_paddingInfo.topBorder*scaleH,
                   (m_modelInfo.width - (m_paddingInfo.rightBorder + m_paddingInfo.leftBorder)) * scaleW, // w
                   (m_modelInfo.height - (m_paddingInfo.bottomBorder - m_paddingInfo.topBorder)) * scaleH); // h

    //TRACE(__func__, " ", bound.x, " ", bound.y, " ", bound.width, " ", bound.height);
    cv::Mat results = padOffData(bound);
    //cv::imwrite("./results.jpg", results);

    results.copyTo(m_scaledUpMask);
    // 3. flatten it.
    m_scaledUpMask = m_scaledUpMask.reshape(1, 1);

    return std::make_pair(bound.width, bound.height);
}



} // namespace aif
