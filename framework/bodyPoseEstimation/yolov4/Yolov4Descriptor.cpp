#include <aif/bodyPoseEstimation/yolov4/Yolov4Descriptor.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>
#include <limits>

namespace {
} // anonymous namespace

namespace rj = rapidjson;

namespace aif {

Yolov4Descriptor::Yolov4Descriptor()
    : Descriptor()
    , m_personCount(0)
{
}

Yolov4Descriptor::~Yolov4Descriptor()
{
}


void Yolov4Descriptor::addPerson(float score, const BBox &bbox)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("persons")) {
        rj::Value persons(rj::kArrayType);
        m_root.AddMember("persons", persons, allocator);
    }

    rj::Value person(rj::kObjectType);
    person.AddMember("score", score, allocator);

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

    m_root["persons"].PushBack(person, allocator);
    m_personCount++;
}

void Yolov4Descriptor::clear()
{
    m_personCount = 0;

    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    m_root.RemoveMember("persons");
    rj::Value persons(rj::kArrayType);
    m_root.AddMember("persons", persons, allocator);
}

}
