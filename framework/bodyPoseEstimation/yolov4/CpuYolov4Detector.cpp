/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/bodyPoseEstimation/yolov4/CpuYolov4Detector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

CpuYolov4Detector::CpuYolov4Detector()
    : Yolov4Detector("yolov4_pytorch.tflite") {}

CpuYolov4Detector::~CpuYolov4Detector() {}

} // namespace aif
