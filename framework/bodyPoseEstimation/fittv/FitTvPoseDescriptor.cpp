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
    if (operationType.rfind("fittv_person_crop", 0) == 0) {
        return addCropBridgeResult(nodeId, result);
    }
    return true;
}

bool FitTvPoseDescriptor::addDetectorOperationResult(
        const std::string& nodeId,
        const std::string& model,
        const std::shared_ptr<Descriptor>& descriptor)
{
    if ((model.rfind("person_yolov4", 0) == 0) ||
        (model.rfind("person_yolov3", 0) == 0)) {
        auto person = std::dynamic_pointer_cast<PersonDetectDescriptor>(descriptor);
        return addPersonDetectorResult(nodeId, person);
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

bool FitTvPoseDescriptor::addCropBridgeResult(
        const std::string& nodeId,
        const std::string& result)
{
    Logi("addCropBridgeResult: ", result);
    auto rects = getCropRects();
    for (int i = 0; i < rects.size(); i++) {
        if (!addCropRect(i+1, rects[i])) {
            return false;
        }
    }

    return true;
}

bool FitTvPoseDescriptor::addPersonDetectorResult(
        const std::string& nodeId,
        const std::shared_ptr<PersonDetectDescriptor> descriptor)
{
    Logi("addPersonDectectorResult: ", descriptor->toStr());
    m_type.addType(NodeType::INFERENCE);
    for (unsigned int i = 0; i < descriptor->getNumBbox(); i++) {
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
    rj::Value jbox(rj::kArrayType);
    jbox.PushBack(static_cast<int>(box.xmin), allocator); // x
    jbox.PushBack(static_cast<int>(box.ymin), allocator); // y
    jbox.PushBack(static_cast<int>(box.width), allocator); //width
    jbox.PushBack(static_cast<int>(box.height), allocator); //height
    jbox.PushBack(score, allocator); //score

    pose.AddMember("bbox", jbox, allocator);
    pose.AddMember("id", m_trackId++, allocator);
    pose.AddMember("timestamp", m_startTime, allocator);
    m_root["poseEstimation"].PushBack(pose, allocator);

    m_boxes.push_back(box);
    return true;
}

bool FitTvPoseDescriptor::addCropRect(int trackId, const cv::Rect& rect)
{
    if (trackId < 1) {
        Loge(__func__, " Wrrong trackId: ", trackId);
        return false;
    }

    size_t index = trackId - 1;

    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("poseEstimation") || m_root["poseEstimation"].Size() <= 0) {
        Loge("not exist bbox");
        return false;
    }

    const auto& poses = m_root["poseEstimation"].GetArray();

    if (trackId <= 0 ||
        poses.Size() < trackId ||
        !poses[index].HasMember("id")) {
        Loge("cannot find trackId: ", trackId);
        return false;
    }

    const auto& pose = poses[index].GetObject();

    rj::Value cropbox(rj::kArrayType);
    cropbox.PushBack(static_cast<int>(rect.x), allocator); // x
    cropbox.PushBack(static_cast<int>(rect.y), allocator); // y
    cropbox.PushBack(static_cast<int>(rect.width), allocator); //width
    cropbox.PushBack(static_cast<int>(rect.height), allocator); //height

    pose.AddMember("cropRect", cropbox, allocator);

    return true;
}

bool FitTvPoseDescriptor::addPose2d(int trackId, const std::vector<std::vector<float>>& keyPoints)
{
    if (trackId < 1) {
        Loge(__func__, " Wrrong trackId: ", trackId);
        return false;
    }

    size_t index = trackId - 1;

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
        !poses[index].HasMember("id")) {
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
        rj::Value keyPoint(rj::kArrayType);
        keyPoint.PushBack(static_cast<int>(pos[1]), allocator); // x
        keyPoint.PushBack(static_cast<int>(pos[2]), allocator); // y
        keyPoint.PushBack(pos[0], allocator); // score
        pose2d.PushBack(keyPoint, allocator);
    }

    pose.AddMember("joints2D", pose2d, allocator);
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

    if (trackId < 1) {
        Loge(__func__, " Wrrong trackId: ", trackId);
        return false;
    }

    size_t index = trackId - 1;

    if (trackId <= 0 ||
        poses.Size() < trackId ||
        !poses[index].HasMember("id")) {
        Loge("cannot find trackId: ", trackId);
        return false;
    }

    const auto& pose = poses[index].GetObject();
    rj::Value pose3d(rj::kArrayType);
    for(auto& pos: joint3ds) {
        rj::Value joint3d(rj::kArrayType);
        joint3d.PushBack(pos.x, allocator); // x
        joint3d.PushBack(pos.y, allocator); // y
        joint3d.PushBack(pos.z, allocator); // z
        pose3d.PushBack(joint3d, allocator);
    }
    pose.AddMember("joints3D", pose3d, allocator);

    rj::Value pose3dPos(rj::kArrayType);
    pose3dPos.PushBack(trajectory.x, allocator); // x
    pose3dPos.PushBack(trajectory.y, allocator); // y
    pose3dPos.PushBack(trajectory.z, allocator); // z
    pose.AddMember("joints3DPosition", pose3dPos, allocator);

    return true;
}

} // end of namespace aif
