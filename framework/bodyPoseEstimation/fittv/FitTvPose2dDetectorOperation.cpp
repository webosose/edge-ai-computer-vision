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
    int trackId = 1;
    for (const auto& image : fitTvDescriptor->getCropImages()) {
        std::shared_ptr<Descriptor> descriptor =
            DetectorFactory::get().getDescriptor(m_model);

        auto pose2dDescriptor = std::dynamic_pointer_cast<Pose2dDescriptor>(descriptor);
        pose2dDescriptor->setTrackId(trackId++);

        //cv::imwrite("/media/result/crop_input.jpg", image);
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
