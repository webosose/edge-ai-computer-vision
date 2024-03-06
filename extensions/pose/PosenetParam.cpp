/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aifex/pose/PosenetParam.h>
#include <aif/tools/Utils.h>

#include <algorithm>
#include <iostream>
#include <sstream>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

PosenetParam::PosenetParam() {}

PosenetParam::~PosenetParam() {}

PosenetParam::PosenetParam(const PosenetParam &other) {
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

PosenetParam::PosenetParam(PosenetParam &&other) noexcept {
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

bool PosenetParam::operator==(const PosenetParam &other) const {
    return (this == &other);
}

bool PosenetParam::operator!=(const PosenetParam &other) const {
    return !operator==(other);
}

// debug
std::ostream &operator<<(std::ostream &os, const PosenetParam &fp) {
    os << "\n{\n";
    os << "}";
    return os;
}

void PosenetParam::trace() {
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

} // namespace aif
