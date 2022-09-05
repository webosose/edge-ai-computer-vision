/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/bodyPoseEstimation/Pose3d/CpuPose3dDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

CpuPose3dDetector::CpuPose3dDetector()
    : Pose3dDetector("FitTV_Pose3D.tflite") {}

CpuPose3dDetector::~CpuPose3dDetector() {}

} // namespace aif
