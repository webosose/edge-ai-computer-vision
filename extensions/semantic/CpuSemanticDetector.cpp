/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aifex/semantic/CpuSemanticDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

CpuSemanticDetector::CpuSemanticDetector()
    : SemanticDetector("deeplabv3_mnv2_dm05_pascal_quant.tflite") {}

CpuSemanticDetector::~CpuSemanticDetector() {}

} // namespace aif
