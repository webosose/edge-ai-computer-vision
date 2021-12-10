#include <aif/movenet/MovenetDescriptor.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>
#include <limits>

namespace {
} // anonymous namespace

namespace rj = rapidjson;

namespace aif {

MovenetDescriptor::MovenetDescriptor()
    : Descriptor()
{
}

MovenetDescriptor::~MovenetDescriptor()
{
}

void MovenetDescriptor::addKeyPoints(
            const std::vector<cv::Point2f>& keyPoints,
            const std::vector<float>& keyPointsScores)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("poses")) {
        rj::Value poses(rj::kArrayType);
        m_root.AddMember("poses", poses, allocator);
    }
    rj::Value person(rj::kObjectType);
    rj::Value points(rj::kArrayType);
    for (int j = 0; j < 17; j++) {
        rj::Value point(rj::kArrayType);
        point.PushBack(keyPoints[j].x, allocator);
        point.PushBack(keyPoints[j].y, allocator);
        points.PushBack(point, allocator);
    }
    person.AddMember("keyPoints", points, allocator);
    m_root["poses"].PushBack(person, allocator);
}

}
