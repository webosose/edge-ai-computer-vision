/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NPU_POSE2D_DETECTOR_H
#define AIF_NPU_POSE2D_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDescriptor.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDetector.h>

namespace aif {

class NpuPose2dDetector : public Pose2dDetector
{
    private:
        NpuPose2dDetector();

    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~NpuPose2dDetector();

    protected:
        void setModelInfo(TfLiteTensor* inputTensor) override;
        t_aif_status fillInputTensor(const cv::Mat& img) override;
        t_aif_status preProcessing() override;
        t_aif_status postProcessing(const cv::Mat& img,
                std::shared_ptr<Descriptor>& descriptor) override;

};

DetectorFactoryRegistration<NpuPose2dDetector, Pose2dDescriptor>
    pose2d_resnet_npu("pose2d_resnet_npu");

} // end of namespace aif

#endif // AIF_NPU_POSE2D_DETECTOR_H
