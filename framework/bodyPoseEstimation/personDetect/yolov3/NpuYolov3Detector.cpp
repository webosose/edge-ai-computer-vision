/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3/NpuYolov3Detector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

NpuYolov3Detector::NpuYolov3Detector()
    : Yolov3Detector("FitTV_Detector_Yolov3.tflite") {}

NpuYolov3Detector::~NpuYolov3Detector() {}

} // namespace aif
