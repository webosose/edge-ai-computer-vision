/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/FaceMaskExtractOperation.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <opencv2/opencv.hpp>

namespace aif {

FaceMaskExtractOperation::FaceMaskExtractOperation(const std::string &id)
: BridgeOperation(id)
{
}

FaceMaskExtractOperation::~FaceMaskExtractOperation()
{
}

bool FaceMaskExtractOperation::runImpl(const std::shared_ptr<NodeInput> &input)
{
    auto &descriptor = input->getDescriptor();
    if (!descriptor)
    {
        Loge(m_id, ": failed to run operaiton (descriptor is null)");
        return false;
    }

    const cv::Mat &image = descriptor->getImage();

    const float scaleW = image.cols;
    const float scaleH = image.rows;

    rj::Document json;
    json.Parse(descriptor->getResult("face_mesh"));

    const rj::Value &facemesh = json["facemesh"];
    const rj::Value &landmarks = facemesh[0]["landmarks"];
    std::vector<cv::Point> landmarkPoly;
    for (rj::SizeType i = 0; i < landmarks.Size(); i += 3)
    {
        auto point_x = landmarks[i + 0].GetDouble() * scaleW;
        auto point_y = landmarks[i + 1].GetDouble() * scaleH;
        landmarkPoly.push_back(cv::Point(point_x, point_y));
    }
    cv::Rect boundRect = cv::boundingRect(landmarkPoly);
    cv::Mat faceMeshCropMat = image(boundRect);

    cv::Mat mask = cv::Mat::zeros(cv::Size(boundRect.width, boundRect.height), CV_8UC1);
    std::vector<std::vector<cv::Point>> contours;
    std::vector<std::vector<cv::Point>> outers;

    const rj::Value &oval = facemesh[0]["oval"];
    std::vector<cv::Point> ovalPoly;
    for (rj::SizeType i = 0; i < oval.Size(); i += 2)
    {
        auto point_x = oval[i + 0].GetDouble() * scaleW;
        auto point_y = oval[i + 1].GetDouble() * scaleH;
        ovalPoly.push_back(cv::Point(point_x - boundRect.x, point_y - boundRect.y));
    }
    outers.push_back(ovalPoly);

    const rj::Value &leftEye = facemesh[0]["leftEye"];
    std::vector<cv::Point> leftEyePoly;
    for (rj::SizeType i = 0; i < leftEye.Size(); i += 2)
    {
        auto point_x = leftEye[i + 0].GetDouble() * scaleW;
        auto point_y = leftEye[i + 1].GetDouble() * scaleH;
        leftEyePoly.push_back(cv::Point(point_x - boundRect.x, point_y - boundRect.y));
    }
    contours.push_back(leftEyePoly);

    const rj::Value &rightEye = facemesh[0]["rightEye"];
    std::vector<cv::Point> rightEyePoly;
    for (rj::SizeType i = 0; i < rightEye.Size(); i += 2)
    {
        auto point_x = rightEye[i + 0].GetDouble() * scaleW;
        auto point_y = rightEye[i + 1].GetDouble() * scaleH;
        rightEyePoly.push_back(cv::Point(point_x - boundRect.x, point_y - boundRect.y));
    }
    contours.push_back(rightEyePoly);

    const rj::Value &mouth = facemesh[0]["mouth"];
    std::vector<cv::Point> mouthPoly;
    for (rj::SizeType i = 0; i < mouth.Size(); i += 2)
    {
        auto point_x = mouth[i + 0].GetDouble() * scaleW;
        auto point_y = mouth[i + 1].GetDouble() * scaleH;
        mouthPoly.push_back(cv::Point(point_x - boundRect.x, point_y - boundRect.y));
    }
    contours.push_back(mouthPoly);

    auto score = facemesh[0]["score"].GetDouble();
    std::cout << "score: " << score << std::endl;

    cv::drawContours(mask, outers, -1, cv::Scalar(255, 255, 255), -1, cv::LINE_AA);
    cv::drawContours(mask, contours, -1, cv::Scalar(0, 0, 0), -1, cv::LINE_AA);

    cv::Mat coloredMask = cv::Mat::zeros(cv::Size(boundRect.width, boundRect.height), CV_8UC3);
    cv::bitwise_and(faceMeshCropMat, faceMeshCropMat, coloredMask, mask = mask);

    descriptor->setImage(coloredMask);

    return true;
}

} // end of namespace aif
