/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/personDetect/yolov3/Yolov3Descriptor.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>
#include <limits>
#include <string>

#include <opencv2/opencv.hpp>

namespace rj = rapidjson;

namespace aif {

Yolov3Descriptor::Yolov3Descriptor()
    : PersonDetectDescriptor()
    , m_faceCount(0)
{
}

Yolov3Descriptor::~Yolov3Descriptor()
{
}

void Yolov3Descriptor::addPerson(float score, const BBox &bbox, double confidenceThreshold, const std::string &dbg_fname)
{
    m_scores.push_back(score); /* only for person detect */
    m_boxes.push_back(bbox);
    m_confidenceThreshold = confidenceThreshold;

    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("persons")) {
        rj::Value persons(rj::kArrayType);
        m_root.AddMember("persons", persons, allocator);
    }

    rj::Value person(rj::kObjectType);
    person.AddMember("score", score, allocator);
    person.AddMember("confidenceThreshold", confidenceThreshold, allocator);

    /* [xmin, ymin, xmax, ymax, c0, c1] */
    rj::Value Bbox(rj::kArrayType);
    Bbox.PushBack(static_cast<int>(bbox.xmin), allocator)
        .PushBack(static_cast<int>(bbox.ymin), allocator)
        .PushBack(static_cast<int>(bbox.xmax), allocator)
        .PushBack(static_cast<int>(bbox.ymax), allocator)
        .PushBack(bbox.c0, allocator)
        .PushBack(bbox.c1, allocator);
    person.AddMember("bbox", Bbox, allocator);

    if (!dbg_fname.empty()) {
        person.AddMember("dbg_fname", dbg_fname, allocator);
        m_dbg_fname = dbg_fname;
    }

    m_root["persons"].PushBack(person, allocator);
    m_personCount++;
}

void Yolov3Descriptor::drawBbox(std::string imgPath)
{
    size_t found = imgPath.find_last_of(".");
    if (found == std::string::npos) {
        Loge(__func__, "Input Image file is wrong");
        return;
    }

    std::string outImagePath = imgPath.substr(0, found) + "_out" + imgPath.substr(found);
    cv::Mat image = cv::imread(imgPath, cv::IMREAD_COLOR);

    if (m_root.HasMember("persons")) {
        // std::cout << "m_root[persons].Size(): " << m_root["persons"].Size() << std::endl; /* TODO: larger... */
        for (int i = 0; i < m_root["persons"].Size(); i++) {
            if (m_root["persons"][i].HasMember("bbox")) {
                auto xmin = m_root["persons"][i]["bbox"][0].GetInt();
                auto ymin = m_root["persons"][i]["bbox"][1].GetInt();
                auto xmax = m_root["persons"][i]["bbox"][2].GetInt();
                auto ymax = m_root["persons"][i]["bbox"][3].GetInt();

                cv::Point lb(xmin, ymin);
                cv::Point tr(xmax, ymax);
                cv::rectangle(image, lb, tr, cv::Scalar(0, 255, 0), 2);

                cv::imwrite(outImagePath, image);
            }
        }
    }

    if (m_root.HasMember("faces")) {
        for (int i = 0; i < m_root["faces"].Size(); i++) {
            if (m_root["faces"][i].HasMember("region")) {
                auto xmin = m_root["faces"][i]["region"][0].GetDouble();
                auto ymin = m_root["faces"][i]["region"][1].GetDouble();
                auto xmax = m_root["faces"][i]["region"][2].GetDouble() + xmin;
                auto ymax = m_root["faces"][i]["region"][3].GetDouble() + ymin;

                cv::Point lb(xmin, ymin);
                cv::Point tr(xmax, ymax);
                cv::rectangle(image, lb, tr, cv::Scalar(255, 0, 0), 2);

                cv::imwrite(outImagePath, image);
            }
        }
    }
}

void Yolov3Descriptor::addFace(float score, float region_x, float region_y, float region_w, float region_h,
                               float lefteye_x, float lefteye_y, float righteye_x, float righteye_y, float nosetip_x,
                               float nosetip_y, float mouth_x, float mouth_y,float leftear_x, float leftear_y,
                               float rightear_x, float rightear_y, double confidenceThreshold)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("faces")) {
        rj::Value faces(rj::kArrayType);
        m_root.AddMember("faces", faces, allocator);
    }

    rj::Value face(rj::kObjectType);
    face.AddMember("score", score, allocator);
    face.AddMember("confidenceThreshold", confidenceThreshold, allocator);

    // region: [x, y, w, h]
    rj::Value region(rj::kArrayType);
    region.PushBack(region_x, allocator)
          .PushBack(region_y, allocator)
          .PushBack(region_w, allocator)
          .PushBack(region_h, allocator);
    face.AddMember("region", region, allocator);

    // lefteye: [x, y]
    rj::Value lefteye(rj::kArrayType);
    lefteye.PushBack(lefteye_x, allocator)
           .PushBack(lefteye_y, allocator);
    face.AddMember("lefteye", lefteye, allocator);

    // righteye: [x, y]
    rj::Value righteye(rj::kArrayType);
    righteye.PushBack(righteye_x, allocator)
            .PushBack(righteye_y, allocator);
    face.AddMember("righteye", righteye, allocator);

    // nosetip: [x, y]
    rj::Value nosetip(rj::kArrayType);
    nosetip.PushBack(nosetip_x, allocator)
           .PushBack(nosetip_y, allocator);
    face.AddMember("nosetip", nosetip, allocator);

    // mouth: [x, y]
    rj::Value mouth(rj::kArrayType);
    mouth.PushBack(mouth_x, allocator)
         .PushBack(mouth_y, allocator);
    face.AddMember("mouth", mouth, allocator);

    // leftear: [x, y]
    rj::Value leftear(rj::kArrayType);
    leftear.PushBack(leftear_x, allocator)
           .PushBack(leftear_y, allocator);
    face.AddMember("leftear", leftear, allocator);

    // rightear: [x, y]
    rj::Value rightear(rj::kArrayType);
    rightear.PushBack(rightear_x, allocator)
            .PushBack(rightear_y, allocator);
    face.AddMember("rightear", rightear, allocator);

    m_root["faces"].PushBack(face, allocator);
    m_faceCount++;
}

void Yolov3Descriptor::clear()
{
    m_personCount = 0;
    m_faceCount = 0;

    m_scores.clear();
    m_boxes.clear();
    m_dbg_fname.clear();

    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    // clear body
    m_root.RemoveMember("persons");
    rj::Value persons(rj::kArrayType);
    m_root.AddMember("persons", persons, allocator);
    // clear face
    m_root.RemoveMember("faces");
    rj::Value faces(rj::kArrayType);
    m_root.AddMember("faces", faces, allocator);
}

} // end of namespace aif