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
{
}

Yolov3Descriptor::~Yolov3Descriptor()
{
}

void Yolov3Descriptor::addPerson(float score, const BBox &bbox, const std::string &dbg_fname)
{
    m_scores.push_back(score); /* only for person detect */
    m_boxes.push_back(bbox);

    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("persons")) {
        rj::Value persons(rj::kArrayType);
        m_root.AddMember("persons", persons, allocator);
    }

    rj::Value person(rj::kObjectType);
    person.AddMember("score", score, allocator);

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

void Yolov3Descriptor::clear()
{
    m_personCount = 0;

    m_scores.clear();
    m_boxes.clear();
    m_dbg_fname.clear();

    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    m_root.RemoveMember("persons");
    rj::Value persons(rj::kArrayType);
    m_root.AddMember("persons", persons, allocator);
}

} // end of namespace aif