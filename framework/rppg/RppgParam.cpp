/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/rppg/RppgParam.h>
#include <aif/tools/Utils.h>

#include <iostream>
#include <sstream>

namespace {
static const char TAG[] = "<FMPARAM>";
} // anonymous namespace

namespace aif {

RppgParam::RppgParam() {}

RppgParam::~RppgParam() {}

RppgParam::RppgParam(const RppgParam &other) {
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

RppgParam::RppgParam(RppgParam &&other) noexcept {
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

bool RppgParam::operator==(const RppgParam &other) const {
    return (this == &other);
}

bool RppgParam::operator!=(const RppgParam &other) const {
    return !operator==(other);
}

// debug
std::ostream &operator<<(std::ostream &os, const RppgParam &fp) {
    os << "\n{\n";
    os << "}";
    return os;
}

void RppgParam::trace() {
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

} // end of namespace aif
