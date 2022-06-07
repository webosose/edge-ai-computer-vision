/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/poseLandmark/CpuPoseLandmarkDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

CpuPoseLandmarkDetector::CpuPoseLandmarkDetector(const std::string &modelPath)
    : PoseLandmarkDetector(modelPath) {}

CpuPoseLandmarkDetector::~CpuPoseLandmarkDetector() {}

LiteCpuPoseLandmarkDetector::LiteCpuPoseLandmarkDetector()
    : CpuPoseLandmarkDetector("pose_landmark_lite.tflite") {}

HeavyCpuPoseLandmarkDetector::HeavyCpuPoseLandmarkDetector()
    : CpuPoseLandmarkDetector("pose_landmark_heavy.tflite") {}

FullCpuPoseLandmarkDetector::FullCpuPoseLandmarkDetector()
    : CpuPoseLandmarkDetector("pose_landmark_full.tflite") {}

} // end of namespace aif
