/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/bodyPoseEstimation/yolov4/NpuYolov4Detector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

NpuYolov4Detector::NpuYolov4Detector()
    : Yolov4Detector("FitTV_Detector.tflite") {}

NpuYolov4Detector::~NpuYolov4Detector() {}

} // namespace aif
