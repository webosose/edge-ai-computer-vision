/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/signLanguageArea/SignLanguageAreaParam.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <iostream>
#include <sstream>
#include <algorithm>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

SignLanguageAreaParam::SignLanguageAreaParam()
{
}

SignLanguageAreaParam::~SignLanguageAreaParam()
{
}

SignLanguageAreaParam::SignLanguageAreaParam(const SignLanguageAreaParam& other)
{
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

SignLanguageAreaParam::SignLanguageAreaParam(SignLanguageAreaParam&& other) noexcept
{
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

bool SignLanguageAreaParam::operator==(const SignLanguageAreaParam& other) const
{
    return (this == &other);
}

bool SignLanguageAreaParam::operator!=(const SignLanguageAreaParam& other) const
{
    return !operator==(other);
}

// debug
std::ostream& operator<<(std::ostream& os, const SignLanguageAreaParam& fp)
{
    os << "\n{\n";
    os << "}";
    return os;
}

void SignLanguageAreaParam::trace()
{
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

}  // end of namespace aif
