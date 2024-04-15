/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_POSE3D_PTQ_DETECTORS_H
#define AIF_POSE3D_PTQ_DETECTORS_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bodyPoseEstimation/pose3d/Pose3dDescriptor.h>
#include <aif/bodyPoseEstimation/pose3d/Pose3dPTQDetector.h>

namespace aif {
/* AIL */
class Pose3dPTQDetector_Low_Pos : public Pose3dPTQDetector<int8_t> {
    private:
        Pose3dPTQDetector_Low_Pos();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~Pose3dPTQDetector_Low_Pos() {}
};
DetectorFactoryRegistration<Pose3dPTQDetector_Low_Pos, Pose3dDescriptor>
    pose3d_videopose3d_ptq_low_pos("pose3d_videopose3d_ptq_pos_low");


class Pose3dPTQDetector_Mid_Pos : public Pose3dPTQDetector<int8_t> {
    private:
        Pose3dPTQDetector_Mid_Pos();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~Pose3dPTQDetector_Mid_Pos() {}
};
DetectorFactoryRegistration<Pose3dPTQDetector_Mid_Pos, Pose3dDescriptor>
    pose3d_videopose3d_ptq_mid_pos("pose3d_videopose3d_ptq_pos_mid");


class Pose3dPTQDetector_Low_Traj : public Pose3dPTQDetector<int8_t> {
    private:
        Pose3dPTQDetector_Low_Traj();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~Pose3dPTQDetector_Low_Traj() {}
};
DetectorFactoryRegistration<Pose3dPTQDetector_Low_Traj, Pose3dDescriptor>
    pose3d_videopose3d_ptq_low_traj("pose3d_videopose3d_ptq_traj_low");


class Pose3dPTQDetector_Mid_Traj : public Pose3dPTQDetector<int8_t> {
    private:
        Pose3dPTQDetector_Mid_Traj();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~Pose3dPTQDetector_Mid_Traj() {}
};
DetectorFactoryRegistration<Pose3dPTQDetector_Mid_Traj, Pose3dDescriptor>
    pose3d_videopose3d_ptq_mid_traj("pose3d_videopose3d_ptq_traj_mid");

/* RTK */
class Pose3dPTQDetector_RTK_Low_Pos : public Pose3dPTQDetector<uint8_t> {
    private:
        Pose3dPTQDetector_RTK_Low_Pos();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~Pose3dPTQDetector_RTK_Low_Pos() {}
};
DetectorFactoryRegistration<Pose3dPTQDetector_RTK_Low_Pos, Pose3dDescriptor>
    pose3d_videopose3d_ptq_rtk_pos_low("pose3d_videopose3d_ptq_pos_rtk_low");


class Pose3dPTQDetector_RTK_Mid_Pos : public Pose3dPTQDetector<uint8_t> {
    private:
        Pose3dPTQDetector_RTK_Mid_Pos();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~Pose3dPTQDetector_RTK_Mid_Pos() {}
};
DetectorFactoryRegistration<Pose3dPTQDetector_RTK_Mid_Pos, Pose3dDescriptor>
    pose3d_videopose3d_ptq_rtk_pos_mid("pose3d_videopose3d_ptq_pos_rtk_mid");


class Pose3dPTQDetector_RTK_Low_Traj : public Pose3dPTQDetector<uint8_t> {
    private:
        Pose3dPTQDetector_RTK_Low_Traj();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~Pose3dPTQDetector_RTK_Low_Traj() {}
};
DetectorFactoryRegistration<Pose3dPTQDetector_RTK_Low_Traj, Pose3dDescriptor>
    pose3d_videopose3d_ptq_rtk_traj_low("pose3d_videopose3d_ptq_traj_rtk_low");


class Pose3dPTQDetector_RTK_Mid_Traj : public Pose3dPTQDetector<uint8_t> {
    private:
        Pose3dPTQDetector_RTK_Mid_Traj();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~Pose3dPTQDetector_RTK_Mid_Traj() {}
};
DetectorFactoryRegistration<Pose3dPTQDetector_RTK_Mid_Traj, Pose3dDescriptor>
    pose3d_videopose3d_ptq_rtk_traj_mid("pose3d_videopose3d_ptq_traj_rtk_mid");

} // end of namespace aif
#endif // AIF_POSE3D_PTQ_DETECTORS_H
