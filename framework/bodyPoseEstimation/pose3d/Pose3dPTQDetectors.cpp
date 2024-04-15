/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/pose3d/Pose3dPTQDetectors.h>

namespace aif {

/* AIL */
Pose3dPTQDetector_Low_Pos::Pose3dPTQDetector_Low_Pos()
: Pose3dPTQDetector("FitTV_Pose3D_GPU_Low_pos_int8_int_int.tflite") {}

Pose3dPTQDetector_Mid_Pos::Pose3dPTQDetector_Mid_Pos()
: Pose3dPTQDetector("FitTV_Pose3D_GPU_Mid_pos_int8_int_int.tflite") {}

Pose3dPTQDetector_Low_Traj::Pose3dPTQDetector_Low_Traj()
: Pose3dPTQDetector("FitTV_Pose3D_GPU_Low_traj_int8_int_int.tflite") {}

Pose3dPTQDetector_Mid_Traj::Pose3dPTQDetector_Mid_Traj()
: Pose3dPTQDetector("FitTV_Pose3D_GPU_Mid_traj_int8_int_int.tflite") {}

/* RTK */
Pose3dPTQDetector_RTK_Low_Pos::Pose3dPTQDetector_RTK_Low_Pos()
: Pose3dPTQDetector("FitTV_Pose3D_GPU_Low_pos_uint_uint.tflite") {}

Pose3dPTQDetector_RTK_Mid_Pos::Pose3dPTQDetector_RTK_Mid_Pos()
: Pose3dPTQDetector("FitTV_Pose3D_GPU_Mid_pos_uint_uint.tflite") {}

Pose3dPTQDetector_RTK_Low_Traj::Pose3dPTQDetector_RTK_Low_Traj()
: Pose3dPTQDetector("FitTV_Pose3D_GPU_Low_traj_uint_uint.tflite") {}

Pose3dPTQDetector_RTK_Mid_Traj::Pose3dPTQDetector_RTK_Mid_Traj()
: Pose3dPTQDetector("FitTV_Pose3D_GPU_Mid_traj_uint_uint.tflite") {}

} // namespace aif
