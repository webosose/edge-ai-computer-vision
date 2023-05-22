/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/fittv/FitTvPose2dDetectorOperation.h>
#include <aif/bodyPoseEstimation/fittv/FitTvPoseDescriptor.h>
#include <aif/base/DetectorFactory.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

namespace aif {

FitTvPose2dDetectorOperation::FitTvPose2dDetectorOperation(const std::string& id)
: DetectorOperation(id)
{
}

FitTvPose2dDetectorOperation::~FitTvPose2dDetectorOperation()
{
}

bool FitTvPose2dDetectorOperation::runImpl(const std::shared_ptr<NodeInput>& input)
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


    int trackId = 1;
    for (const auto& image : fitTvDescriptor->getCropImages()) {
        std::shared_ptr<Descriptor> descriptor =
            DetectorFactory::get().getDescriptor(m_model);

        auto pose2dDescriptor = std::dynamic_pointer_cast<Pose2dDescriptor>(descriptor);
        if (pose2dDescriptor == nullptr) {
            Loge(__func__, "failed to convert Descriptor to Pose2dDescriptor");
            return false;
        }

        auto pose2dDetector = std::dynamic_pointer_cast<Pose2dDetector>(m_detector);
        if (pose2dDetector == nullptr) {
            Loge(__func__, "failed to convert Detector to Pose2dDetector");
            return false;
        }

        auto cropData = fitTvDescriptor->getCropData(); // get scale data
        auto cropBbox = fitTvDescriptor->getCropBbox(); // get fixedBbox

        if (cropData.size() >= trackId) { /* having cropscale, useUDP */
            Logd(__func__, " setCropData: ", cropBbox[trackId-1], " use UDP");
            pose2dDetector->setCropData(cropData[trackId-1], cropBbox[trackId-1], true);
        } else if (cropBbox.size() >= trackId) { /* having only fixedBbox, not useUDP */
            Logd(__func__, " setCropData: ", cropBbox[trackId-1], " NOT use UDP");
            Scale noScale;
            pose2dDetector->setCropData(noScale, cropBbox[trackId-1], false);
        }

        pose2dDescriptor->setTrackId(trackId++);

        t_aif_status res = m_detector->detect(image, descriptor);
        if (res != kAifOk) {
            Loge(m_id, ": failed to detect ", m_model);
            return false;
        }

        fitTvDescriptor->addDetectorOperationResult(m_id, m_model, descriptor);
    }

   return true;
}

} // end of namespace aif
