#include <aif/bodyPoseEstimation/personDetect/yolov4/Yolov4Descriptor.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>
#include <limits>
#include <string>

#include <opencv2/opencv.hpp>

namespace {
} // anonymous namespace

namespace rj = rapidjson;

namespace aif {

Yolov4Descriptor::Yolov4Descriptor()
    : PersonDetectDescriptor()
{
}

Yolov4Descriptor::~Yolov4Descriptor()
{
}


void Yolov4Descriptor::addPerson(float score, const BBox &bbox, double confidenceThreshold, const std::string &dbg_fname)
{
    m_scores.push_back(score);
    m_boxes.push_back(bbox);

    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("persons")) {
        rj::Value persons(rj::kArrayType);
        m_root.AddMember("persons", persons, allocator);
    }

    rj::Value person(rj::kObjectType);
    person.AddMember("score", score, allocator);
    person.AddMember("confidenceThreshold", confidenceThreshold, allocator);

    /* [xmin, ymin, xmax, ymax, width, height, c_x, c_y] */
    rj::Value Bbox(rj::kArrayType);
    Bbox.PushBack(bbox.xmin, allocator)
        .PushBack(bbox.ymin, allocator)
        .PushBack(bbox.xmax, allocator)
        .PushBack(bbox.ymax, allocator)
        .PushBack(bbox.width, allocator)
        .PushBack(bbox.height, allocator)
        .PushBack(bbox.c_x, allocator)
        .PushBack(bbox.c_y, allocator);
    person.AddMember("bbox", Bbox, allocator);

    if (!dbg_fname.empty()) {
        person.AddMember("dbg_fname", dbg_fname, allocator);
        m_dbg_fname = dbg_fname;
    }

    m_root["persons"].PushBack(person, allocator);
    m_personCount++;
    m_IsBodyDetect = true;
}

void Yolov4Descriptor::drawBbox(std::string imgPath)
{
    size_t found = imgPath.find_last_of(".");
    if (found == std::string::npos) {
        Loge(__func__, "Input Image file is wrong");
        return;
    }

    std::string outImagePath = imgPath.substr(0, found) + "_out" + imgPath.substr(found);
    cv::Mat image = cv::imread(imgPath, cv::IMREAD_COLOR);

    if (m_root.HasMember("persons")) {
        for (int i = 0; i < m_root["persons"].Size(); i++) {
            if (m_root["persons"][i].HasMember("bbox")) {
                auto xmin = m_root["persons"][i]["bbox"][0].GetDouble();
                auto ymin = m_root["persons"][i]["bbox"][1].GetDouble();
                auto xmax = m_root["persons"][i]["bbox"][2].GetDouble();
                auto ymax = m_root["persons"][i]["bbox"][3].GetDouble();

                cv::Point lb(xmax, ymax);
                cv::Point tr(xmin, ymin);
                cv::rectangle(image, lb, tr, cv::Scalar(0, 255, 0), 3);

                cv::imwrite(outImagePath, image);
            }
        }
    }
}

void Yolov4Descriptor::clear()
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

}
