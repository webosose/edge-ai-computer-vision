/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aifex/semantic/SemanticParam.h>
#include <aif/tools/Utils.h>

#include <algorithm>
#include <iostream>
#include <sstream>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

SemanticParam::SemanticParam() {}

SemanticParam::~SemanticParam() {}

SemanticParam::SemanticParam(const SemanticParam &other) {
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

SemanticParam::SemanticParam(SemanticParam &&other) noexcept {
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

bool SemanticParam::operator==(const SemanticParam &other) const {
    return (this == &other);
}

bool SemanticParam::operator!=(const SemanticParam &other) const {
    return !operator==(other);
}

// debug
std::ostream &operator<<(std::ostream &os, const SemanticParam &fp) {
    os << "\n{\n";
    os << "}";
    return os;
}

void SemanticParam::trace() {
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

} // namespace aif
