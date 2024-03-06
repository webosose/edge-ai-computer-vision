/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aifex/text/CpuTextDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

CpuTextDetector320::CpuTextDetector320()
    : TextDetector("paddleocr-model-320.tflite", false) {}

CpuTextDetector320::~CpuTextDetector320() {}

/////////////////////////////////////////////////////////////////////
CpuTextDetector256::CpuTextDetector256()
    : TextDetector("paddleocr-model-256.tflite", false) {}

CpuTextDetector256::~CpuTextDetector256() {}


/////////////////////////////////////////////////////////////////////
CpuTextDetector320_V2::CpuTextDetector320_V2()
    : TextDetector("paddleocr_320_320_float32.tflite", true) {}

CpuTextDetector320_V2::~CpuTextDetector320_V2() {}


/////////////////////////////////////////////////////////////////////
CpuTextDetector256_V2::CpuTextDetector256_V2()
    : TextDetector("paddleocr_256_256_float32.tflite", true) {}

CpuTextDetector256_V2::~CpuTextDetector256_V2() {}


} // namespace aif
