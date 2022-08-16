#include <aif/bodyPoseEstimation/pose2d/Pose2dDescriptor.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>
#include <limits>

namespace {
} // anonymous namespace

namespace rj = rapidjson;

namespace aif {

Pose2dDescriptor::Pose2dDescriptor()
    : Descriptor()
    , m_poseCount(0)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    rj::Value poses(rj::kArrayType);
    m_root.AddMember("poses", poses, allocator);
}

Pose2dDescriptor::~Pose2dDescriptor()
{
}

void Pose2dDescriptor::addKeyPoints(const std::vector<std::vector<float>>& keyPoints)
{
    m_keyPoints = keyPoints;

    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("poses")) {
        rj::Value poses(rj::kArrayType);
        m_root.AddMember("poses", poses, allocator);
    }

    rj::Value person(rj::kObjectType);
    rj::Value points(rj::kArrayType);
    for (int i = 0; i < keyPoints.size(); i++) {
        rj::Value point(rj::kArrayType);
        for (int j = 0; j < keyPoints[i].size(); j++) {
            point.PushBack(keyPoints[i][j], allocator);
        }
        points.PushBack(point, allocator);
    }
    person.AddMember("keypoints", points, allocator);
    m_root["poses"].PushBack(person, allocator);
    m_poseCount += keyPoints.size();
}

const std::vector<std::vector<float>>& Pose2dDescriptor::getKeyPoints() const
{
    return m_keyPoints;
}

}
