/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CPU_POSE2D_DETECTORS_H
#define AIF_CPU_POSE2D_DETECTORS_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDescriptor.h>
#include <aif/bodyPoseEstimation/pose2d/CpuPose2dDetector.h>

namespace aif {

class CpuPose2dDetector_Low : public CpuPose2dDetector {
    private:
        CpuPose2dDetector_Low();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~CpuPose2dDetector_Low() {}
};
DetectorFactoryRegistration<CpuPose2dDetector_Low, Pose2dDescriptor>
    pose2d_resnet_low_cpu("pose2d_resnet_low_cpu");


class CpuPose2dDetector_Mid : public CpuPose2dDetector {
    private:
        CpuPose2dDetector_Mid();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~CpuPose2dDetector_Mid() {}
};
DetectorFactoryRegistration<CpuPose2dDetector_Mid, Pose2dDescriptor>
    pose2d_resnet_mid_cpu("pose2d_resnet_mid_cpu");

} // end of namespace aif

#endif // AIF_CPU_POSE2D_DETECTORS_H
