/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3_v2/NpuYolov3V2Detector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

NpuYolov3V2Detector::NpuYolov3V2Detector()
    : Yolov3V2Detector("FitTV_Detector_Yolov3_V2.tflite") {}

NpuYolov3V2Detector::~NpuYolov3V2Detector() {}

} // namespace aif
