/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_POSE2D_PTQ_DETECTORS_H
#define AIF_POSE2D_PTQ_DETECTORS_H

#include <aif/bodyPoseEstimation/pose2d/Pose2dPTQDetector.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDescriptor.h>

namespace aif {
/* AIR */
class Pose2dPTQDetector_Low : public Pose2dPTQDetector {
    private:
        Pose2dPTQDetector_Low();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~Pose2dPTQDetector_Low() {}
};
DetectorFactoryRegistration<Pose2dPTQDetector_Low, Pose2dDescriptor>
    pose2d_resnet_low_ptq("pose2d_resnet_low_ptq");


class Pose2dPTQDetector_Mid : public Pose2dPTQDetector {
    private:
        Pose2dPTQDetector_Mid();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~Pose2dPTQDetector_Mid() {}
};
DetectorFactoryRegistration<Pose2dPTQDetector_Mid, Pose2dDescriptor>
    pose2d_resnet_mid_ptq("pose2d_resnet_mid_ptq");

} // end of namespace aif
#endif // AIF_POSE2D_PTQ_DETECTORS_H
