/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/movenet/MovenetParam.h>
#include <aif/tools/Utils.h>

#include <algorithm>
#include <iostream>
#include <sstream>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

MovenetParam::MovenetParam() {}

MovenetParam::~MovenetParam() {}

MovenetParam::MovenetParam(const MovenetParam &other) {
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

MovenetParam::MovenetParam(MovenetParam &&other) noexcept {
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

bool MovenetParam::operator==(const MovenetParam &other) const {
    return (this == &other);
}

bool MovenetParam::operator!=(const MovenetParam &other) const {
    return !operator==(other);
}

// debug
std::ostream &operator<<(std::ostream &os, const MovenetParam &fp) {
    os << "\n{\n";
    os << "}";
    return os;
}

void MovenetParam::trace() {
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

} // namespace aif
