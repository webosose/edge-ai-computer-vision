/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/fittv/FitTvPose3dDetectorOperation.h>
#include <aif/bodyPoseEstimation/fittv/FitTvPoseDescriptor.h>
#include <aif/base/DetectorFactory.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

namespace aif {

FitTvPose3dDetectorOperation::FitTvPose3dDetectorOperation(const std::string& id)
: DetectorOperation(id)
{
}

FitTvPose3dDetectorOperation::~FitTvPose3dDetectorOperation()
{
}

bool FitTvPose3dDetectorOperation::runImpl(const std::shared_ptr<NodeInput>& input)
{
    if (!m_detector) {
        Loge(m_id, ": detector is null");
        return false;
    }

    std::shared_ptr<PipeDescriptor> pipeDescriptor = input->getDescriptor();
    if (!pipeDescriptor) {
        Loge(m_id, ": node descriptor is null");
        return false;
    }

    auto fitTvDescriptor = std::dynamic_pointer_cast<FitTvPoseDescriptor>(pipeDescriptor);
    if (fitTvDescriptor == nullptr) {
        Loge(__func__, "failed to convert PipeDescriptor to FitTvPoseDescriptor");
        return false;
    }

    if (fitTvDescriptor->getPose3dInputs().size() == 0) {
        Loge(m_id, ": failed to get pose3d inputs from FitTvPoseDescriptor");
        return false;
    }

    int trackId = 1;
    for (const auto& pose3dInput: fitTvDescriptor->getPose3dInputs()) {
        std::shared_ptr<Descriptor> descriptor =
            DetectorFactory::get().getDescriptor(m_model);

        auto pose3dDescriptor = std::dynamic_pointer_cast<Pose3dDescriptor>(descriptor);
        if (pose3dDescriptor == nullptr) {
            Loge(__func__, "failed to convert Descriptor to Pose3dDescriptor");
            return false;
        }

        auto pose3dDetector = std::dynamic_pointer_cast<Pose3dDetector>(m_detector);
        if (pose3dDetector == nullptr) {
            Loge(__func__, "failed to convert Detector to Pose3dDetector");
            return false;
        }

        pose3dDescriptor->setTrackId(trackId++);
        pose3dDetector->setNumSkippedInput(fitTvDescriptor->getNumSkippedFrames());

        t_aif_status res = m_detector->detect(pose3dInput, descriptor);
        if (res != kAifOk) {
            Loge(m_id, ": failed to detect ", m_model);
            return false;
        }

        fitTvDescriptor->addDetectorOperationResult(m_id, m_model, descriptor);
    }

   return true;
}

} // end of namespace aif
