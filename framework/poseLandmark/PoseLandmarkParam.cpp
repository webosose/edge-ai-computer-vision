/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/poseLandmark/PoseLandmarkParam.h>
#include <aif/tools/Utils.h>

#include <algorithm>
#include <iostream>
#include <sstream>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

PoseLandmarkParam::PoseLandmarkParam() {}

PoseLandmarkParam::~PoseLandmarkParam() {}

PoseLandmarkParam::PoseLandmarkParam(const PoseLandmarkParam &other) {
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

PoseLandmarkParam::PoseLandmarkParam(PoseLandmarkParam &&other) noexcept {
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

bool PoseLandmarkParam::operator==(const PoseLandmarkParam &other) const {
    return (this == &other);
}

bool PoseLandmarkParam::operator!=(const PoseLandmarkParam &other) const {
    return !operator==(other);
}

// debug
std::ostream &operator<<(std::ostream &os, const PoseLandmarkParam &fp) {
    os << "\n{\n";
    os << "}";
    return os;
}

void PoseLandmarkParam::trace() {
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

} // namespace aif
