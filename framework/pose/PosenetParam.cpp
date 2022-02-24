#include <aif/pose/PosenetParam.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <iostream>
#include <sstream>
#include <algorithm>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

PosenetParam::PosenetParam()
{
}

PosenetParam::~PosenetParam()
{
}

PosenetParam::PosenetParam(const PosenetParam& other)
{
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

PosenetParam::PosenetParam(PosenetParam&& other) noexcept
{
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

PosenetParam& PosenetParam::operator=(const PosenetParam& other)
{
    // TRACE(TAG, "ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }
    return *this;
}

PosenetParam& PosenetParam::operator=(PosenetParam&& other) noexcept
{
    // TRACE(TAG, "MOVE ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }
    return *this;
}

bool PosenetParam::operator==(const PosenetParam& other) const
{
    return (this == &other);
}

bool PosenetParam::operator!=(const PosenetParam& other) const
{
    return !operator==(other);
}

// debug
std::ostream& operator<<(std::ostream& os, const PosenetParam& fp)
{
    os << "\n{\n";
    os << "}";
    return os;
}

void PosenetParam::trace()
{
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

}
