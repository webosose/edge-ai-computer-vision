/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/fittv/FitTvPose2d3dBridgeOperation.h>
#include <aif/bodyPoseEstimation/fittv/FitTvPoseDescriptor.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

namespace aif {

FitTvPose2d3dBridgeOperation::FitTvPose2d3dBridgeOperation(const std::string& id)
: BridgeOperation(id)
{
}

FitTvPose2d3dBridgeOperation::~FitTvPose2d3dBridgeOperation()
{
}

bool FitTvPose2d3dBridgeOperation::runImpl(const std::shared_ptr<NodeInput>& input)
{
    auto& descriptor = input->getDescriptor();
    if (!descriptor) {
        Loge(m_id, ": failed to run operaiton (descriptor is null)");
        return false;
    }
    std::shared_ptr<FitTvPoseDescriptor> fdescriptor=
        std::dynamic_pointer_cast<FitTvPoseDescriptor>(input->getDescriptor());

    for (const auto& keyPoints : fdescriptor->getKeyPoints()) {
        std::vector<float> joints2d;
        for (const auto& keyPoint : keyPoints) {
            joints2d.push_back(keyPoint[1]);
            joints2d.push_back(keyPoint[2]);
        }

        cv::Mat joints2d_mat(41, 2, CV_32F, joints2d.data());
        cv::Mat joints2d_matd;
        joints2d_mat.convertTo(joints2d_matd, CV_64F );
        fdescriptor->addPose3dInput(joints2d_matd);
    }
    return true;
}


} // end of namespace aif
