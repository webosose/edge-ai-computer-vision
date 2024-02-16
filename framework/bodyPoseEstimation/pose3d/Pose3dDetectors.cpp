/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/pose3d/Pose3dDetectors.h>

namespace aif {
NpuPose3dDetector_V1::NpuPose3dDetector_V1()
: Pose3dDetector("FitTV_Pose3D_V1.tflite") {}

NpuPose3dDetector_V2_Low_Pos::NpuPose3dDetector_V2_Low_Pos()
: Pose3dDetector("FitTV_Pose3D_V2_Low_Pos.tflite") {}

NpuPose3dDetector_V2_Low_Traj::NpuPose3dDetector_V2_Low_Traj()
: Pose3dDetector("FitTV_Pose3D_V2_Low_Traj.tflite") {}

NpuPose3dDetector_V2_Mid_Pos::NpuPose3dDetector_V2_Mid_Pos()
: Pose3dDetector("FitTV_Pose3D_V2_Mid_Pos.tflite") {}

NpuPose3dDetector_V2_Mid_Traj::NpuPose3dDetector_V2_Mid_Traj()
: Pose3dDetector("FitTV_Pose3D_V2_Mid_Traj.tflite") {}

} // namespace aif
