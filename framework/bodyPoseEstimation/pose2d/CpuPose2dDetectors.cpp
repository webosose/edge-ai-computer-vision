/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/pose2d/CpuPose2dDetectors.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dParam.h>

namespace aif {

CpuPose2dDetector_Low::CpuPose2dDetector_Low()
: CpuPose2dDetector("FitTV_Pose2D_GPU_Low.tflite") {}

CpuPose2dDetector_Mid::CpuPose2dDetector_Mid()
: CpuPose2dDetector("FitTV_Pose2D_GPU_Mid.tflite") {}

} // namespace aif
