/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/handLandmark/CpuHandLandmarkDetector.h>
#include <aif/log/Logger.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

CpuHandLandmarkDetector::CpuHandLandmarkDetector(const std::string &modelPath)
    : HandLandmarkDetector(modelPath) {}

CpuHandLandmarkDetector::~CpuHandLandmarkDetector() {}

LiteCpuHandLandmarkDetector::LiteCpuHandLandmarkDetector()
    : CpuHandLandmarkDetector("hand_landmark_lite.tflite") {}

FullCpuHandLandmarkDetector::FullCpuHandLandmarkDetector()
    : CpuHandLandmarkDetector("hand_landmark_full.tflite") {}

} // namespace aif
