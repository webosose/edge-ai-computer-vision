/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/faceMesh/FaceMeshParam.h>
#include <aif/tools/Utils.h>

#include <iostream>
#include <sstream>

namespace {
static const char TAG[] = "<FMPARAM>";
} // anonymous namespace

namespace aif {

FaceMeshParam::FaceMeshParam() {}

FaceMeshParam::~FaceMeshParam() {}

FaceMeshParam::FaceMeshParam(const FaceMeshParam &other) {
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

FaceMeshParam::FaceMeshParam(FaceMeshParam &&other) noexcept {
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

bool FaceMeshParam::operator==(const FaceMeshParam &other) const {
    return (this == &other);
}

bool FaceMeshParam::operator!=(const FaceMeshParam &other) const {
    return !operator==(other);
}

// debug
std::ostream &operator<<(std::ostream &os, const FaceMeshParam &fp) {
    os << "\n{\n";
    os << "}";
    return os;
}

void FaceMeshParam::trace() {
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

} // end of namespace aif
