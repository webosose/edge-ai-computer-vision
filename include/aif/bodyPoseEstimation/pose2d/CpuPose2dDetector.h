/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CPU_POSE2D_DETECTOR_H
#define AIF_CPU_POSE2D_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDescriptor.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDetector.h>
#include <aif/bodyPoseEstimation/pose2d/PostProcess.h>


namespace aif {

class CpuPose2dDetector : public Pose2dDetector, public std::enable_shared_from_this<CpuPose2dDetector>
{
    private:
        CpuPose2dDetector();

    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~CpuPose2dDetector();
        std::shared_ptr<CpuPose2dDetector> get_shared_ptr() {return shared_from_this();}
    protected:
        void setModelInfo(TfLiteTensor* inputTensor) override;
        t_aif_status fillInputTensor(const cv::Mat& img) override;
        t_aif_status preProcessing() override;
        t_aif_status postProcessing(const cv::Mat& img,
                std::shared_ptr<Descriptor>& descriptor) override;
        void normalizeImage(cv::Mat& img) const;
};

DetectorFactoryRegistration<CpuPose2dDetector, Pose2dDescriptor>
    pose2d_resnet_cpu("pose2d_resnet_cpu");

} // end of namespace aif

#endif // AIF_CPU_POSE2D_DETECTOR_H
