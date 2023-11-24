/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NPU_POSE3D_DETECTORV2S_H
#define AIF_NPU_POSE3D_DETECTORV2S_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bodyPoseEstimation/pose3d/Pose3dDescriptor.h>
#include <aif/bodyPoseEstimation/pose3d/NpuPose3dDetectorV2.h>

namespace aif {

class NpuPose3dDetector_V2_Low_Pos : public NpuPose3dDetectorV2 {
    private:
        NpuPose3dDetector_V2_Low_Pos();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~NpuPose3dDetector_V2_Low_Pos() {}
};
DetectorFactoryRegistration<NpuPose3dDetector_V2_Low_Pos, Pose3dDescriptor>
    pose3d_videopose3d_v2_pos_low("pose3d_videopose3d_v2_pos_low");


class NpuPose3dDetector_V2_Low_Traj : public NpuPose3dDetectorV2 {
    private:
        NpuPose3dDetector_V2_Low_Traj();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~NpuPose3dDetector_V2_Low_Traj() {}
};
DetectorFactoryRegistration<NpuPose3dDetector_V2_Low_Traj, Pose3dDescriptor>
    pose3d_videopose3d_v2_traj_low("pose3d_videopose3d_v2_traj_low");


class NpuPose3dDetector_V2_Mid_Pos : public NpuPose3dDetectorV2 {
    private:
        NpuPose3dDetector_V2_Mid_Pos();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~NpuPose3dDetector_V2_Mid_Pos() {}
};
DetectorFactoryRegistration<NpuPose3dDetector_V2_Mid_Pos, Pose3dDescriptor>
    pose3d_videopose3d_v2_pos_mid("pose3d_videopose3d_v2_pos_mid");


class NpuPose3dDetector_V2_Mid_Traj : public NpuPose3dDetectorV2 {
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

#endif // AIF_NPU_POSE3D_DETECTORV2S_H
