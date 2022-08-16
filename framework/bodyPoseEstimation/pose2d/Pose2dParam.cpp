/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/pose2d/Pose2dParam.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <iostream>
#include <sstream>
#include <algorithm>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

Pose2dParam::Pose2dParam()
{
}

Pose2dParam::~Pose2dParam()
{
}

Pose2dParam::Pose2dParam(const Pose2dParam& other)
{
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

Pose2dParam::Pose2dParam(Pose2dParam&& other) noexcept
{
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

Pose2dParam& Pose2dParam::operator=(const Pose2dParam& other)
{
    // TRACE(TAG, "ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }
    return *this;
}

Pose2dParam& Pose2dParam::operator=(Pose2dParam&& other) noexcept
{
    // TRACE(TAG, "MOVE ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }
    return *this;
}

bool Pose2dParam::operator==(const Pose2dParam& other) const
{
    return (this == &other);
}

bool Pose2dParam::operator!=(const Pose2dParam& other) const
{
    return !operator==(other);
}

// debug
std::ostream& operator<<(std::ostream& os, const Pose2dParam& fp)
{
    os << "\n{\n";
    os << "}";
    return os;
}

void Pose2dParam::trace()
{
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

}
