#include <aif/movenet/MovenetParam.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <iostream>
#include <sstream>
#include <algorithm>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

MovenetParam::MovenetParam()
{
}

MovenetParam::~MovenetParam()
{
}

MovenetParam::MovenetParam(const MovenetParam& other)
{
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

MovenetParam::MovenetParam(MovenetParam&& other) noexcept
{
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

MovenetParam& MovenetParam::operator=(const MovenetParam& other)
{
    // TRACE(TAG, "ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }
    return *this;
}

MovenetParam& MovenetParam::operator=(MovenetParam&& other) noexcept
{
    // TRACE(TAG, "MOVE ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }
    return *this;
}

bool MovenetParam::operator==(const MovenetParam& other) const
{
    return (this == &other); 
}

bool MovenetParam::operator!=(const MovenetParam& other) const
{
    return !operator==(other);
}

// debug
std::ostream& operator<<(std::ostream& os, const MovenetParam& fp)
{
    os << "\n{\n";
    os << "}";
    return os;
}

void MovenetParam::trace()
{
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

}
