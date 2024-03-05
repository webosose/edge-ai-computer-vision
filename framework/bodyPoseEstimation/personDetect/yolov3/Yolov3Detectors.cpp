/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/personDetect/yolov3/Yolov3Detectors.h>

namespace aif {

NpuYolov3V1Detector::NpuYolov3V1Detector()
: Yolov3Detector("FitTV_Detector_Yolov3_V1.tflite", static_cast<int>(VersionType::MODEL_V1)) {}

NpuYolov3V2Detector::NpuYolov3V2Detector()
: Yolov3Detector("FitTV_Detector_Yolov3_V2.tflite", static_cast<int>(VersionType::MODEL_V2)) {}

} // namespace aif
