/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/pose2d/Pose2dPTQDetectors.h>

namespace aif {

/* AIF PTQ */
Pose2dPTQDetector_Low::Pose2dPTQDetector_Low()
: Pose2dPTQDetector("FitTV_Pose2D_GPU_Low_int8_uint_uint.tflite", typeid(uint8_t)) {}

Pose2dPTQDetector_Mid::Pose2dPTQDetector_Mid()
: Pose2dPTQDetector("FitTV_Pose2D_GPU_Mid_int8_uint_uint.tflite", typeid(uint8_t)) {}

} // namespace aif
