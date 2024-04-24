/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NPU_POSE3D_DETECTORS_H
#define AIF_NPU_POSE3D_DETECTORS_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bodyPoseEstimation/pose3d/Pose3dDescriptor.h>
#include <aif/bodyPoseEstimation/pose3d/Pose3dDetector.h>

namespace aif {

class NpuPose3dDetector_V1 : public Pose3dDetector {
    private:
        NpuPose3dDetector_V1();

    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~NpuPose3dDetector_V1() {}
};
DetectorFactoryRegistration<NpuPose3dDetector_V1, Pose3dDescriptor>
    pose3d_videopose3d_v1_npu("pose3d_videopose3d_v1_npu");

class NpuPose3dDetector_V2_Low_Pos : public Pose3dDetector {
    private:
        NpuPose3dDetector_V2_Low_Pos();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~NpuPose3dDetector_V2_Low_Pos() {}
};
DetectorFactoryRegistration<NpuPose3dDetector_V2_Low_Pos, Pose3dDescriptor>
    pose3d_videopose3d_v2_pos_low("pose3d_videopose3d_v2_pos_low");


class NpuPose3dDetector_V2_Low_Traj : public Pose3dDetector {
    private:
        NpuPose3dDetector_V2_Low_Traj();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~NpuPose3dDetector_V2_Low_Traj() {}
};
DetectorFactoryRegistration<NpuPose3dDetector_V2_Low_Traj, Pose3dDescriptor>
    pose3d_videopose3d_v2_traj_low("pose3d_videopose3d_v2_traj_low");


class NpuPose3dDetector_V2_Mid_Pos : public Pose3dDetector {
    private:
        NpuPose3dDetector_V2_Mid_Pos();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~NpuPose3dDetector_V2_Mid_Pos() {}
};
DetectorFactoryRegistration<NpuPose3dDetector_V2_Mid_Pos, Pose3dDescriptor>
    pose3d_videopose3d_v2_pos_mid("pose3d_videopose3d_v2_pos_mid");


class NpuPose3dDetector_V2_Mid_Traj : public Pose3dDetector {
    private:
        NpuPose3dDetector_V2_Mid_Traj();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~NpuPose3dDetector_V2_Mid_Traj() {}
};
DetectorFactoryRegistration<NpuPose3dDetector_V2_Mid_Traj, Pose3dDescriptor>
    pose3d_videopose3d_v2_traj_mid("pose3d_videopose3d_v2_traj_mid");

} // end of namespace aif

#endif // AIF_POSE3D_DETECTORS_H
