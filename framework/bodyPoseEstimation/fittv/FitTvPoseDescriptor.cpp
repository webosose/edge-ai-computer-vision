/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/fittv/FitTvPoseDescriptor.h>
#include <aif/pipe/NodeType.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace aif {

FitTvPoseDescriptor::FitTvPoseDescriptor()
: PipeDescriptor()
{
}

FitTvPoseDescriptor::~FitTvPoseDescriptor()
{
}

bool FitTvPoseDescriptor::addBridgeOperationResult(
        const std::string& id, const std::string& result)
{
    return true;
}

bool FitTvPoseDescriptor::addDetectorOperationResult(
        const std::string& id,
        const std::string& model,
        const std::shared_ptr<Descriptor>& descriptor)
{
    if (model.rfind("person_yolov4", 0) == 0) {
        auto yolov4 = std::dynamic_pointer_cast<Yolov4Descriptor>(descriptor);
        return addPersonDetectorResult(id, yolov4);
    }
    else if (model.rfind("pose2d_resnet", 0) == 0) {
        auto pose2d = std::dynamic_pointer_cast<Pose2dDescriptor>(descriptor);
        return addPose2dDetectorResult(id, pose2d);
    }
    return false;
}

bool FitTvPoseDescriptor::addPersonDetectorResult(
        const std::string& id,
        const std::shared_ptr<Yolov4Descriptor> descriptor)
{
    Logi("addPersonDectectorResult: ", descriptor->toStr());
    m_type.addType(NodeType::INFERENCE);

    return true;
}

bool FitTvPoseDescriptor::addPose2dDetectorResult(
        const std::string& id,
        const std::shared_ptr<Pose2dDescriptor> descriptor)
{
    Logi("addPose2dDetectorResult: ", descriptor->toStr());
    m_type.addType(NodeType::INFERENCE);

    return true;
}

} // end of namespace aif
