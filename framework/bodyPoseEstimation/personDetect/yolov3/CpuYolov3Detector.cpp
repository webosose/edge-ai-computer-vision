/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3/CpuYolov3Detector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

CpuYolov3Detector::CpuYolov3Detector()
    : Yolov3Detector("yolov3_float32.tflite") {}

CpuYolov3Detector::~CpuYolov3Detector() {}

} // namespace aif
