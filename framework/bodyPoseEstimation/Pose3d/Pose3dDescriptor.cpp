#include <aif/bodyPoseEstimation/Pose3d/Pose3dDescriptor.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>
#include <limits>

namespace {
} // anonymous namespace

namespace rj = rapidjson;

namespace aif {

Pose3dDescriptor::Pose3dDescriptor()
    : Descriptor()
{
}

Pose3dDescriptor::~Pose3dDescriptor()
{
}

void
Pose3dDescriptor::addJoints3D(const std::vector<Joint3D> &joints3d,
                              const Joint3D &joint3dPos)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("poses3d")) {
        rj::Value poses3d(rj::kArrayType);
        m_root.AddMember("poses3d", poses3d, allocator);
    }

    rj::Value person(rj::kObjectType);
    rj::Value points(rj::kArrayType);

    for (int i = 0; i < joints3d.size(); i++) {
        rj::Value point(rj::kArrayType);
        point.PushBack(joints3d[i].x, allocator);
        point.PushBack(joints3d[i].y, allocator);
        point.PushBack(joints3d[i].z, allocator);

        points.PushBack(point, allocator);
    }

    person.AddMember("joints3d", points, allocator);

    rj::Value pointsPos(rj::kArrayType);
    pointsPos.PushBack(joint3dPos.x, allocator);
    pointsPos.PushBack(joint3dPos.y, allocator);
    pointsPos.PushBack(joint3dPos.z, allocator);

    person.AddMember("joint3dPos", pointsPos, allocator);

    m_root["poses3d"].PushBack(person, allocator);

    m_3dResult = joints3d;
    m_trajectory = joint3dPos;
}


}
