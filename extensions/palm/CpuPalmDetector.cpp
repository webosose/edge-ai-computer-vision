/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/palm/CpuPalmDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

CpuPalmDetector::CpuPalmDetector(const std::string &modelPath)
    : PalmDetector(modelPath) {}

CpuPalmDetector::~CpuPalmDetector() {}

LiteCpuPalmDetector::LiteCpuPalmDetector()
    : CpuPalmDetector("palm_detection_lite.tflite") {}

FullCpuPalmDetector::FullCpuPalmDetector()
    : CpuPalmDetector("palm_detection_full.tflite") {}

} // namespace aif
