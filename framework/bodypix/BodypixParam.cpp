/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodypix/BodypixParam.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <algorithm>
#include <iostream>
#include <sstream>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

BodypixParam::BodypixParam() {}

BodypixParam::~BodypixParam() {}

BodypixParam::BodypixParam(const BodypixParam &other) {
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

BodypixParam::BodypixParam(BodypixParam &&other) noexcept {
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

// BodypixParam& BodypixParam::operator=(const BodypixParam& other)
// {
//     // TRACE(TAG, "ASSIGNMENT OPERATOR....");
//     if (this == &other) {
//         return *this;
//     }
//     return *this;
// }

// BodypixParam& BodypixParam::operator=(BodypixParam&& other) noexcept
// {
//     // TRACE(TAG, "MOVE ASSIGNMENT OPERATOR....");
//     if (this == &other) {
//         return *this;
//     }
//     return *this;
// }

bool BodypixParam::operator==(const BodypixParam &other) const {
    return (this == &other);
}

bool BodypixParam::operator!=(const BodypixParam &other) const {
    return !operator==(other);
}

// debug
std::ostream &operator<<(std::ostream &os, const BodypixParam &fp) {
    os << "\n{\n";
    os << "}";
    return os;
}

void BodypixParam::trace() {
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

} // namespace aif
