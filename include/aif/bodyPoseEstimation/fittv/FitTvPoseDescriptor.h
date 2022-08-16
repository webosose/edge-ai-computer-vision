/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_FITTV_POSE_DESCRIPTOR_H
#define AIF_FITTV_POSE_DESCRIPTOR_H

#include <aif/pipe/PipeDescriptor.h>
#include <aif/pipe/PipeDescriptorFactoryRegistration.h>
#include <aif/bodyPoseEstimation/yolov4/Yolov4Descriptor.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDescriptor.h>
#include <aif/base/Types.h>

#include <memory>

namespace aif {

class FitTvPoseDescriptor : public PipeDescriptor
{
    private:
        FitTvPoseDescriptor();
    public:
        virtual ~FitTvPoseDescriptor();

        template<typename T>
        friend class PipeDescriptorFactoryRegistration;

        bool addBridgeOperationResult(
            const std::string& id,
            const std::string& result) override;

        bool addDetectorOperationResult(
            const std::string& id,
            const std::string& model,
            const std::shared_ptr<Descriptor>& descriptor) override;

    private:
        bool addPersonDetectorResult(
                const std::string& id,
                const std::shared_ptr<Yolov4Descriptor> descriptor);
        bool addPose2dDetectorResult(
                const std::string& id,
                const std::shared_ptr<Pose2dDescriptor> descriptor);
};

PipeDescriptorFactoryRegistration<FitTvPoseDescriptor>
    fittv_pose_descriptor("fittv_pose");

} // end of namespace aif

#endif // AIF_FITTV_POSE_DESCRIPTOR_H
