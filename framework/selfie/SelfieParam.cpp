#include <aif/log/Logger.h>
#include <aif/selfie/SelfieParam.h>
#include <aif/tools/Utils.h>

#include <algorithm>
#include <iostream>
#include <sstream>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

SelfieParam::SelfieParam() {}

SelfieParam::~SelfieParam() {}

SelfieParam::SelfieParam(const SelfieParam &other) {
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

SelfieParam::SelfieParam(SelfieParam &&other) noexcept {
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

bool SelfieParam::operator==(const SelfieParam &other) const {
    return (this == &other);
}

bool SelfieParam::operator!=(const SelfieParam &other) const {
    return !operator==(other);
}

// debug
std::ostream &operator<<(std::ostream &os, const SelfieParam &fp) {
    os << "\n{\n";
    os << "}";
    return os;
}

void SelfieParam::trace() {
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

} // namespace aif
