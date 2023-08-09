/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3_yc/NpuYolov3YCDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

NpuYolov3YCDetector::NpuYolov3YCDetector()
    : Yolov3YCDetector("FitTV_Detector_Yolov3_YC.tflite") {}

NpuYolov3YCDetector::~NpuYolov3YCDetector() {}

} // namespace aif
