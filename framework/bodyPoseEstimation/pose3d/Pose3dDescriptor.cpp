#include <aif/bodyPoseEstimation/pose3d/Pose3dDescriptor.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace {
} // anonymous namespace

namespace rj = rapidjson;

namespace aif {

Pose3dDescriptor::Pose3dDescriptor()
: Descriptor()
, m_trackId(-1)
, m_3dResult()
, m_trajectory()
{
}

Pose3dDescriptor::~Pose3dDescriptor()
{
}

void Pose3dDescriptor::addJoints3D(const std::vector<Joint3D> &joints3d)
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
    m_root["poses3d"].PushBack(person, allocator);
    m_3dResult = joints3d;
}

void Pose3dDescriptor::addTraj3D(const Joint3D &joint3dTraj)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("poses3d")) {
        rj::Value poses3d(rj::kArrayType);
        m_root.AddMember("poses3d", poses3d, allocator);
    }

    rj::Value person(rj::kObjectType);
    rj::Value pointsTraj(rj::kArrayType);
    pointsTraj.PushBack(joint3dTraj.x, allocator);
    pointsTraj.PushBack(joint3dTraj.y, allocator);
    pointsTraj.PushBack(joint3dTraj.z, allocator);

    person.AddMember("joint3dTraj", pointsTraj, allocator);
    m_root["poses3d"].PushBack(person, allocator);
    m_trajectory = joint3dTraj;
}


}
