/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/mname/CpuMNameDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

CpuMNameDetector::CpuMNameDetector()
    : MNameDetector("cpu_model_name") {}

CpuMNameDetector::~CpuMNameDetector() {}

} // namespace aif
