/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/signLanguageArea/CpuSignLanguageAreaDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

CpuSignLanguageAreaDetector::CpuSignLanguageAreaDetector()
    : SignLanguageAreaDetector("signlanguagearea_mb2_ssd_lite_float32.tflite") {}

CpuSignLanguageAreaDetector::~CpuSignLanguageAreaDetector() {}


} // namespace aif
