/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3_v1/NpuYolov3V1Detector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

NpuYolov3V1Detector::NpuYolov3V1Detector()
    : Yolov3V1Detector("FitTV_Detector_Yolov3_V1.tflite") {}

NpuYolov3V1Detector::~NpuYolov3V1Detector() {}

} // namespace aif
