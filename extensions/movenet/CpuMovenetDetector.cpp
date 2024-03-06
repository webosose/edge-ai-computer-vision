/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aifex/movenet/CpuMovenetDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>

#include <sstream>
#include <stdexcept>

namespace aif {

CpuMovenetDetector::CpuMovenetDetector()
    : MovenetDetector("movenet_single_pose_thunder_ptq.tflite") {}

CpuMovenetDetector::~CpuMovenetDetector() {}

} // namespace aif
