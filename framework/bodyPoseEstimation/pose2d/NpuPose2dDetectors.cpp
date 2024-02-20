/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/pose2d/NpuPose2dDetectors.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dParam.h>

namespace aif {

NpuPose2dDetector_V1::NpuPose2dDetector_V1()
: NpuPose2dDetector("FitTV_Pose2D_V1.tflite") {}

NpuPose2dDetector_V2_Low::NpuPose2dDetector_V2_Low()
: NpuPose2dDetector("FitTV_Pose2D_V2_Low.tflite") {}

NpuPose2dDetector_V2_Mid::NpuPose2dDetector_V2_Mid()
: NpuPose2dDetector("FitTV_Pose2D_V2_Mid.tflite") {}

} // namespace aif
