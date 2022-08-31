/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/fittv/FitTvPoseDescriptor.h>
#include <aif/bodyPoseEstimation/fittv/FitTvPoseDescriptorRegistration.h>
#include <aif/pipe/NodeType.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace aif {

FitTvPoseDescriptor::FitTvPoseDescriptor()
: PipeDescriptor()
, m_trackId(1)
{
}

FitTvPoseDescriptor::~FitTvPoseDescriptor()
{
}

bool FitTvPoseDescriptor::addBridgeOperationResult(
        const std::string& nodeId,
        const std::string& operationType,
        const std::string& result)
{
    return true;
}

bool FitTvPoseDescriptor::addDetectorOperationResult(
        const std::string& nodeId,
        const std::string& model,
        const std::shared_ptr<Descriptor>& descriptor)
{
    if (model.rfind("person_yolov4", 0) == 0) {
        auto yolov4 = std::dynamic_pointer_cast<Yolov4Descriptor>(descriptor);
        return addPersonDetectorResult(nodeId, yolov4);
    }
    else if (model.rfind("pose2d_resnet", 0) == 0) {
        auto pose2d = std::dynamic_pointer_cast<Pose2dDescriptor>(descriptor);
        return addPose2dDetectorResult(nodeId, pose2d);
    }
    else if (model.rfind("pose3d", 0) == 0) {
        auto pose3d = std::dynamic_pointer_cast<Pose3dDescriptor>(descriptor);
        return addPose3dDetectorResult(nodeId, pose3d);
    }
    return false;
}

bool FitTvPoseDescriptor::addPersonDetectorResult(
        const std::string& nodeId,
        const std::shared_ptr<Yolov4Descriptor> descriptor)
{
    Logi("addPersonDectectorResult: ", descriptor->toStr());
    m_type.addType(NodeType::INFERENCE);
    for (int i = 0; i < descriptor->getNumBbox(); i++) {
        if (!addBBox(descriptor->getScore(i), descriptor->getBbox(i))) {
            return false;
        }
    }
    return true;
}

bool FitTvPoseDescriptor::addPose2dDetectorResult(
        const std::string& nodeId,
        const std::shared_ptr<Pose2dDescriptor> descriptor)
{
    Logi("addPose2dDetectorResult: ", descriptor->toStr());
    m_type.addType(NodeType::INFERENCE);
    return addPose2d(descriptor->getTrackId(), descriptor->getKeyPoints());
}

bool FitTvPoseDescriptor::addPose3dDetectorResult(
        const std::string& nodeId,
        const std::shared_ptr<Pose3dDescriptor> descriptor)
{
    Logi("addPose3dDetectorResult: ", descriptor->toStr());
    m_type.addType(NodeType::INFERENCE);
    return addPose3d(descriptor->getTrackId(),
            descriptor->getPose3dResult(),
            descriptor->getTrajectory());
}

bool FitTvPoseDescriptor::addBBox(float score, const BBox& box)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("poseEstimation")) {
        rj::Value poses(rj::kArrayType);
        m_root.AddMember("poseEstimation", poses, allocator);
    }
    rj::Value pose(rj::kObjectType);
    rj::Value jbox(rj::kObjectType);
    jbox.AddMember("score", score, allocator);
    jbox.AddMember("x", static_cast<int>(box.xmin), allocator);
    jbox.AddMember("y", static_cast<int>(box.ymin), allocator);
    jbox.AddMember("width", static_cast<int>(box.width), allocator);
    jbox.AddMember("height", static_cast<int>(box.height), allocator);

    pose.AddMember("bbox", jbox, allocator);
    pose.AddMember("trackId", m_trackId++, allocator);
    m_root["poseEstimation"].PushBack(pose, allocator);

    m_boxes.push_back(box);
    return true;
}

bool FitTvPoseDescriptor::addPose2d(int trackId, const std::vector<std::vector<float>>& keyPoints)
{
    int index = trackId - 1;
    if (m_keyPoints.size() != index) {
        Loge("cannot add keyPoints by trackId: ", trackId);
        return false;
    }

    m_keyPoints.push_back(keyPoints);
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("poseEstimation") || m_root["poseEstimation"].Size() <= 0) {
        Loge("not exist bbox");
        return false;
    }

    const auto& poses = m_root["poseEstimation"].GetArray();

    if (trackId <= 0 ||
        poses.Size() < trackId ||
        !poses[index].HasMember("trackId")) {
        Loge("cannot find trackId: ", trackId);
        return false;
    }

    const auto& pose = poses[index].GetObject();
    rj::Value pose2d(rj::kArrayType);

    for(auto& pos: m_keyPoints[index]) {
        if (index < m_cropRects.size()) {
            pos[1] += m_cropRects[index].x;
            pos[2] += m_cropRects[index].y;
        }
        rj::Value keyPoint(rj::kObjectType);
        keyPoint.AddMember("score", pos[0], allocator);
        keyPoint.AddMember("x", static_cast<int>(pos[1]), allocator);
        keyPoint.AddMember("y", static_cast<int>(pos[2]), allocator);
        pose2d.PushBack(keyPoint, allocator);
    }

    pose.AddMember("pose2d", pose2d, allocator);
    return true;
}

bool FitTvPoseDescriptor::addPose3d(
        int trackId,
        const std::vector<Joint3D>& joint3ds,
        const Joint3D& trajectory)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("poseEstimation") || m_root["poseEstimation"].Size() <= 0) {
        Loge("not exist bbox");
        return false;
    }

    const auto& poses = m_root["poseEstimation"].GetArray();

    int index = trackId - 1;
    if (trackId <= 0 ||
        poses.Size() < trackId ||
        !poses[index].HasMember("trackId")) {
        Loge("cannot find trackId: ", trackId);
        return false;
    }

    const auto& pose = poses[index].GetObject();
    rj::Value pose3d(rj::kArrayType);
    for(auto& pos: joint3ds) {
        rj::Value joint3d(rj::kObjectType);
        joint3d.AddMember("x", pos.x, allocator);
        joint3d.AddMember("y", pos.y, allocator);
        joint3d.AddMember("z", pos.z, allocator);
        pose3d.PushBack(joint3d, allocator);
    }
    pose.AddMember("pose3d", pose3d, allocator);

    rj::Value pose3dPos(rj::kObjectType);
    pose3dPos.AddMember("x", trajectory.x, allocator);
    pose3dPos.AddMember("y", trajectory.y, allocator);
    pose3dPos.AddMember("z", trajectory.z, allocator);
    pose.AddMember("pose3dPos", pose3dPos, allocator);

    return true;
}

} // end of namespace aif
