/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/bodyPoseEstimation/pose3d/NpuPose3dDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

NpuPose3dDetector::NpuPose3dDetector()
    : Pose3dDetector("FitTV_Pose3D_V1.tflite") {}

NpuPose3dDetector::~NpuPose3dDetector() {}

} // namespace aif
