/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/pose3d/NpuPose3dDetectorV2s.h>

namespace aif {

NpuPose3dDetector_V2_Low_Pos::NpuPose3dDetector_V2_Low_Pos()
: NpuPose3dDetectorV2("FitTV_Pose3D_V2_Low_Pos.tflite") {}

NpuPose3dDetector_V2_Low_Traj::NpuPose3dDetector_V2_Low_Traj()
: NpuPose3dDetectorV2("FitTV_Pose3D_V2_Low_Traj.tflite") {}

NpuPose3dDetector_V2_Mid_Pos::NpuPose3dDetector_V2_Mid_Pos()
: NpuPose3dDetectorV2("FitTV_Pose3D_V2_Mid_Pos.tflite") {}

NpuPose3dDetector_V2_Mid_Traj::NpuPose3dDetector_V2_Mid_Traj()
: NpuPose3dDetectorV2("FitTV_Pose3D_V2_Mid_Traj.tflite") {}

} // namespace aif