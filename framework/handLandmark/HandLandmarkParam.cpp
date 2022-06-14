#include <aif/handLandmark/HandLandmarkParam.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <iostream>
#include <sstream>
#include <algorithm>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

HandLandmarkParam::HandLandmarkParam()
{
}

HandLandmarkParam::~HandLandmarkParam()
{
}

HandLandmarkParam::HandLandmarkParam(const HandLandmarkParam& other)
{
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

HandLandmarkParam::HandLandmarkParam(HandLandmarkParam&& other) noexcept
{
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

bool HandLandmarkParam::operator==(const HandLandmarkParam& other) const
{
    return (this == &other); 
}

bool HandLandmarkParam::operator!=(const HandLandmarkParam& other) const
{
    return !operator==(other);
}

// debug
std::ostream& operator<<(std::ostream& os, const HandLandmarkParam& fp)
{
    os << "\n{\n";
    os << "}";
    return os;
}

void HandLandmarkParam::trace()
{
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

}
