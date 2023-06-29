/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3_yc_v1/NpuYolov3YCV1Detector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

NpuYolov3YCV1Detector::NpuYolov3YCV1Detector()
    : Yolov3YCV1Detector("FitTV_Detector_Yolov3_YC_V1.tflite") {}

NpuYolov3YCV1Detector::~NpuYolov3YCV1Detector() {}

} // namespace aif
