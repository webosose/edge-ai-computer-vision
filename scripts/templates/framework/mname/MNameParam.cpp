/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/mname/MNameParam.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <iostream>
#include <sstream>
#include <algorithm>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

MNameParam::MNameParam()
{
}

MNameParam::~MNameParam()
{
}

MNameParam::MNameParam(const MNameParam& other)
{
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

MNameParam::MNameParam(MNameParam&& other) noexcept
{
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

MNameParam& MNameParam::operator=(const MNameParam& other)
{
    // TRACE(TAG, "ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }
    return *this;
}

MNameParam& MNameParam::operator=(MNameParam&& other) noexcept
{
    // TRACE(TAG, "MOVE ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }
    return *this;
}

bool MNameParam::operator==(const MNameParam& other) const
{
    return (this == &other); 
}

bool MNameParam::operator!=(const MNameParam& other) const
{
    return !operator==(other);
}

// debug
std::ostream& operator<<(std::ostream& os, const MNameParam& fp)
{
    os << "\n{\n";
    os << "}";
    return os;
}

void MNameParam::trace()
{
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

}
