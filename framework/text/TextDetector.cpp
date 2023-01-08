/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/text/TextDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>
#include <vector>

#include "clipper.hpp"

namespace aif {

using namespace ClipperLib;

TextDetector::TextDetector(const std::string& modelPath, bool useNHWC)
    : Detector(modelPath)
    , m_useNHWC(useNHWC)
{
}

TextDetector::~TextDetector()
{
}

std::shared_ptr<DetectorParam> TextDetector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<TextParam>();
    return param;
}

void TextDetector::setModelInfo(TfLiteTensor* inputTensor)
{
    if (m_useNHWC) {
        m_modelInfo.batchSize = inputTensor->dims->data[0];
        m_modelInfo.height = inputTensor->dims->data[1];
        m_modelInfo.width = inputTensor->dims->data[2];
        m_modelInfo.channels = inputTensor->dims->data[3];
    } else {
        m_modelInfo.batchSize = inputTensor->dims->data[0];
        m_modelInfo.channels = inputTensor->dims->data[1];
        m_modelInfo.height = inputTensor->dims->data[2];
        m_modelInfo.width = inputTensor->dims->data[3];
    }
    TRACE("input_size: ", m_modelInfo.inputSize);
    TRACE("batch_size: ", m_modelInfo.batchSize);
    TRACE("height:", m_modelInfo.height);
    TRACE("width: ", m_modelInfo.width);
    TRACE("channels: ", m_modelInfo.channels);
}

t_aif_status TextDetector::fillInputTensor(const cv::Mat& img)/* override*/
{
    try {
        float* inputTensor = m_interpreter->typed_input_tensor<float>(0);
        cv::Mat blob = cv::dnn::blobFromImage(img, 1.0/255.0,
                cv::Size(m_modelInfo.width, m_modelInfo.height), cv::Scalar(103.939, 116.779, 123.68), false, false);

        if (m_useNHWC) {
            for(int c = 0; c < m_modelInfo.channels; ++c) {
                for(int y = 0; y < m_modelInfo.height; ++y) {
                    for(int x = 0; x < m_modelInfo.width; ++x) {
                        inputTensor[y * (m_modelInfo.width * m_modelInfo.channels) + x * m_modelInfo.channels + c]
                            = blob.at<float>(c * m_modelInfo.height * m_modelInfo.width + y * m_modelInfo.width + x);
                    }
                }
            }
        } else {
            memcpy(inputTensor, blob.data, blob.total() * blob.elemSize());
        }
    } catch(const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return kAifError;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return kAifError;
    }

    return kAifOk;
}

t_aif_status TextDetector::preProcessing()
{
    return kAifOk;
}

t_aif_status TextDetector::postProcessing(const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor)
{
    const std::vector<int> &outputs = m_interpreter->outputs();
    TfLiteTensor *output0 = m_interpreter->tensor(outputs[0]);
    float* output = reinterpret_cast<float*>(output0->data.data);


    cv::Mat probabilityMap = cv::Mat::zeros(m_modelInfo.width, m_modelInfo.height, CV_32FC1);
    for (int y = 0; y < m_modelInfo.height; ++y) {
        for (int x = 0; x < m_modelInfo.width; ++x) {
            probabilityMap.at<float>(cv::Point(x, y)) = *(output + m_modelInfo.width*y + x);
        }
    }
    std::vector<cv::Rect> resultRects;
    convertOutputToBoxes(probabilityMap, resultRects, img.size());

    std::shared_ptr<TextDescriptor> textDescriptor = std::dynamic_pointer_cast<TextDescriptor>(descriptor);
    textDescriptor->addTextRects(resultRects);

    return kAifOk;
}


void TextDetector::convertOutputToBoxes(cv::Mat& probabilityMap, std::vector<cv::Rect>& resultRects, cv::Size imgOriginalSize)
{
    std::shared_ptr<TextParam> param = std::dynamic_pointer_cast<TextParam>(m_param);

    float imgWidth = imgOriginalSize.width;
    float imgHeight = imgOriginalSize.height;
    float scaleRatioX = m_modelInfo.width/imgWidth;
    float scaleRatioY = m_modelInfo.height/imgHeight;

    cv::threshold(probabilityMap, probabilityMap, 0.3, 255, cv::THRESH_TOZERO);

    cv::Mat binaryMap;
    probabilityMap.convertTo(binaryMap, CV_8UC1, 255);

    std::vector<std::vector<cv::Point>> contours;
    std::vector<std::vector<cv::Point>> processedContours;
    cv::findContours(binaryMap, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    for (auto const& contour : contours) {
        std::vector<cv::Point> approx;
        double epsilon = 0.01 * cv::arcLength(contour, true);
        cv::approxPolyDP(contour, approx, epsilon, true);
        if (approx.size() < 4) continue;

        if (boxScoreFast(probabilityMap, approx, param->m_boxThreshold)) {
            continue;
        }

        approx = unclip(approx, 2.0);

        cv::RotatedRect miniBox = cv::minAreaRect(approx);
        if (std::max(miniBox.size.height, miniBox.size.width) < 5) continue;

        for (size_t j = 0; j < approx.size(); j++) {
            approx[j].x = (int)(approx[j].x / scaleRatioX);
            approx[j].y = (int)(approx[j].y / scaleRatioY);
        }

        processedContours.push_back(approx);
    }

    for (auto const& contour : processedContours) {
        cv::Rect rect = cv::boundingRect(contour);
        rect.x = clip(rect.x, 0, imgWidth);
        rect.y = clip(rect.y, 0, imgHeight);
        rect.width = uint(clip(rect.x + rect.width, 0, imgWidth - 1) - rect.x);
        rect.height = uint(clip(rect.y + rect.height, 0, imgHeight - 1) - rect.y);
        resultRects.push_back(rect);
    }
}


bool TextDetector::boxScoreFast(cv::Mat& binaryMap, std::vector<cv::Point> approx, float boxThresh)
{
    float xmin = m_modelInfo.width-1, xmax = 0, ymin = m_modelInfo.height-1, ymax = 0;
    for (auto const& point : approx)
    {
        xmin = (point.x < xmin) ? point.x : xmin;
        xmax = (point.x > xmax) ? point.x : xmax;
        ymin = (point.y < ymin) ? point.y : ymin;
        ymax = (point.y > ymax) ? point.y : ymax;
    }

    cv::Mat mask = cv::Mat::zeros(cv::Size(xmax - xmin + 1, ymax - ymin + 1), CV_8UC1);
    cv::Mat meanBitmap = cv::Mat::zeros(cv::Size(xmax - xmin + 1, ymax - ymin + 1), CV_32FC1);

    for (auto &point : approx) {
        point.x = point.x - xmin;
        point.y = point.y - ymin;
    }

    cv::fillPoly(mask, approx, 1);

    for (int i = ymin; i < ymax; ++i) {
        for (int j = xmin; j < xmax; ++j) {
            meanBitmap.at<float>(cv::Point(j - xmin, i-ymin)) = binaryMap.at<float>(cv::Point(j, i));
        }
    }

    float score = cv::mean(meanBitmap, mask).val[0];
    return (boxThresh > score) ? true : false;
}

std::vector<cv::Point> TextDetector::unclip(const std::vector<cv::Point> &box, float unclipRatio)
{
    std::vector<cv::Point> expandedContours;
    Path polygon(box.size());
    Paths solution;
    for (size_t i = 0; i < box.size(); i++) {
        polygon[i].X = box[i].x;
        polygon[i].Y = box[i].y;
    }
    double area = abs(ClipperLib::Area(polygon));
    float polygonLength = cv::arcLength(box, true);
    float distance = area * unclipRatio / polygonLength;

    ClipperOffset co;
    co.AddPath(polygon, jtRound, etClosedPolygon);
    co.Execute(solution, distance);

    for (auto const& intPoint: solution[0]) {
        expandedContours.push_back(cv::Point(intPoint.X, intPoint.Y));
    }
    return expandedContours;

}

int TextDetector::clip(float n, float lower, float upper)
{
    return std::max(lower, std::min(n, upper));
}

} // end of namespace aif
