#include <aif/semantic/SemanticParam.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <iostream>
#include <sstream>
#include <algorithm>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

SemanticParam::SemanticParam()
{
}

SemanticParam::~SemanticParam()
{
}

SemanticParam::SemanticParam(const SemanticParam& other)
{
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

SemanticParam::SemanticParam(SemanticParam&& other) noexcept
{
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

SemanticParam& SemanticParam::operator=(const SemanticParam& other)
{
    // TRACE(TAG, "ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }
    return *this;
}

SemanticParam& SemanticParam::operator=(SemanticParam&& other) noexcept
{
    // TRACE(TAG, "MOVE ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }
    return *this;
}

bool SemanticParam::operator==(const SemanticParam& other) const
{
    return (this == &other); 
}

bool SemanticParam::operator!=(const SemanticParam& other) const
{
    return !operator==(other);
}

// debug
std::ostream& operator<<(std::ostream& os, const SemanticParam& fp)
{
    os << "\n{\n";
    os << "}";
    return os;
}

void SemanticParam::trace()
{
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

}
