/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/FaceMeshExtractOperation.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <opencv2/opencv.hpp>

namespace aif {

FaceMeshExtractOperation::FaceMeshExtractOperation(const std::string &id)
: BridgeOperation(id)
{
}

FaceMeshExtractOperation::~FaceMeshExtractOperation()
{
}

bool FaceMeshExtractOperation::runImpl(const std::shared_ptr<NodeInput> &input)
{
    auto &descriptor = input->getDescriptor();
    if (!descriptor)
    {
        Loge(m_id, ": failed to run operaiton (descriptor is null)");
        return false;
    }

    std::shared_ptr<FaceMeshExtractOperationConfig> config
        = std::dynamic_pointer_cast<FaceMeshExtractOperationConfig>(m_config);
    if (config == nullptr)
    {
        Loge(m_id, ": failed to convert BridgeOperationConfig to FaceMeshExtractOperationConfig");
        return false;
    }

    const cv::Mat &image = descriptor->getImage();

    const float scaleW = image.cols;
    const float scaleH = image.rows;

    rj::Document json;
    if(config->getTargetId() == "") {
        Loge(m_id, ": failed to get targetId. Face Mesh Extract Operation must need TargetID.");
        return false;
    }
    json.Parse(descriptor->getResult(config->getTargetId()));

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

    try {
        cv::Rect fullImageRect(0, 0, image.cols, image.rows);
        boundRect = fullImageRect & boundRect;
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

        cv::drawContours(mask, outers,   -1, cv::Scalar(255, 255, 255), -1, cv::LINE_AA);
        cv::drawContours(mask, contours, -1, cv::Scalar(0, 0, 0),       -1, cv::LINE_AA);

        cv::Mat coloredMask = cv::Mat::zeros(cv::Size(boundRect.width, boundRect.height), CV_8UC1);
        cv::bitwise_and(faceMeshCropMat, faceMeshCropMat, coloredMask, mask = mask);
        // imwrite("face_mesh.png", coloredMask);

        if (config->getRGBextractOn()) // RGB Extraction for rPPG
        {
            int count = 0;
            float r,g,b;
            for (int i=0; i<mask.rows; i++){
                for (int j=0; j<mask.cols; j++) {
                    if (mask.at<unsigned char>(i, j) == 255) { // mask == 255 -> True
                        cv::Vec3b bgr = coloredMask.at<cv::Vec3b>(i, j);
                        r += bgr[2];
                        g += bgr[1];
                        b += bgr[0];
                        count++;
                    }
                }
            }
            r /= count;
            g /= count;
            b /= count;

            rj::Document doc;
            doc.SetObject();
            rj::Document::AllocatorType& allocator = doc.GetAllocator();
            rj::Value faceMeshRGB(rj::kArrayType);
            faceMeshRGB.PushBack(r, allocator);
            faceMeshRGB.PushBack(g, allocator);
            faceMeshRGB.PushBack(b, allocator);
            doc.GetObject().AddMember("meshData", faceMeshRGB, allocator);
            descriptor->addBridgeOperationResult(m_id,
                                                 m_config->getType(),
                                                 jsonObjectToString(doc.GetObject()));
        }
        else
        {
            descriptor->setImage(coloredMask);
        }

    } catch(const cv::Exception& e) {
        Loge(__func__, " Error: ", e.msg);
        Loge(__func__, " boundingRect: ", boundRect.x, ", ", boundRect.y,
                                    ", ", boundRect.width, ", ", boundRect.height);
        Loge(__func__, " image: ", image.cols, ", ", image.rows);
        return false;
    } catch(const std::exception& e) {
        Loge(__func__, " Error: ", e.what());
        return false;
    } catch(...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return false;
    }

    return true;
}

} // end of namespace aif