/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NPU_POSE2D_DETECTORS_H
#define AIF_NPU_POSE2D_DETECTORS_H

#include <aif/bodyPoseEstimation/pose2d/NpuPose2dDetector.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDescriptor.h>

namespace aif {

class NpuPose2dDetector_V1 : public NpuPose2dDetector {
    private:
        NpuPose2dDetector_V1();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~NpuPose2dDetector_V1() {}
};
DetectorFactoryRegistration<NpuPose2dDetector_V1, Pose2dDescriptor>
    pose2d_resnet_v1_npu("pose2d_resnet_v1_npu");


class NpuPose2dDetector_V2_Low : public NpuPose2dDetector {
    private:
        NpuPose2dDetector_V2_Low();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~NpuPose2dDetector_V2_Low() {}
};
DetectorFactoryRegistration<NpuPose2dDetector_V2_Low, Pose2dDescriptor>
    pose2d_resnet_v2_low_npu("pose2d_resnet_v2_low_npu");


class NpuPose2dDetector_V2_Mid : public NpuPose2dDetector {
    private:
        NpuPose2dDetector_V2_Mid();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~NpuPose2dDetector_V2_Mid() {}
};
DetectorFactoryRegistration<NpuPose2dDetector_V2_Mid, Pose2dDescriptor>
    pose2d_resnet_v2_mid_npu("pose2d_resnet_v2_mid_npu");

} // end of namespace aif

#endif // AIF_NPU_POSE2D_DETECTORS_H