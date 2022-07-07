/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/face/YuNetFaceParam.h>
#include <aif/log/Logger.h>

#include <rapidjson/document.h>
namespace rj = rapidjson;

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace


namespace aif {

YuNetFaceParam::YuNetFaceParam()
    : scoreThreshold(0.7f)
    , nmsThreshold(0.3f)
    , topK(5000)
{
}


YuNetFaceParam::YuNetFaceParam(const YuNetFaceParam& other)
    : scoreThreshold{other.scoreThreshold}
    , nmsThreshold{other.nmsThreshold}
    , topK{other.topK}
{
}

YuNetFaceParam::YuNetFaceParam(YuNetFaceParam&& other) noexcept
    : scoreThreshold{std::exchange(other.scoreThreshold, 0.7f)}
    , nmsThreshold{std::exchange(other.nmsThreshold, 0.3f)}
    , topK{std::exchange(other.topK, 5000)}
{
}

YuNetFaceParam& YuNetFaceParam::operator=(const YuNetFaceParam& other)
{
    if (this == &other) {
        return *this;
    }
    scoreThreshold = other.scoreThreshold;
    nmsThreshold = other.nmsThreshold;
    topK = other.topK;

    return *this;
}

YuNetFaceParam& YuNetFaceParam::operator=(YuNetFaceParam&& other) noexcept
{
    if (this == &other) {
        return *this;
    }
    scoreThreshold = std::exchange(other.scoreThreshold, 0.7f);
    nmsThreshold = std::exchange(other.nmsThreshold, 0.3f);
    topK = std::exchange(other.topK, 5000);

    return *this;
}

bool YuNetFaceParam::operator==(const YuNetFaceParam& other) const
{
    return (
        (std::abs(scoreThreshold - other.scoreThreshold) < aif::EPSILON) &&
        (std::abs(nmsThreshold - other.nmsThreshold) < aif::EPSILON) &&
        (topK == other.topK)
    );
}

bool YuNetFaceParam::operator!=(const YuNetFaceParam& other) const
{
    return !operator==(other);
}

// debug
std::ostream& operator<<(std::ostream& os, const YuNetFaceParam& fp)
{
    os << "\n{\n";
    os << "\tscoreThreshold: " << fp.scoreThreshold << ",\n";
    os << "\tnmsThreshold: " << fp.nmsThreshold << ",\n";
    os << "\ttopK: " << fp.topK<< ",\n";
    os << "}";

    return os;
}

void YuNetFaceParam::trace()
{
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

t_aif_status YuNetFaceParam::fromJson(const std::string& param)
{
    t_aif_status res = DetectorParam::fromJson(param);
    rj::Document payload;
    payload.Parse(param.c_str());
    if (payload.HasMember("modelParam")) {
        rj::Value& modelParam = payload["modelParam"];
        if (modelParam.HasMember("scoreThreshold"))
            scoreThreshold = modelParam["scoreThreshold"].GetFloat();

        if (modelParam.HasMember("scoreThreshold"))
            nmsThreshold = modelParam["nmsThreshold"].GetFloat();

        if (modelParam.HasMember("topK"))
            topK = modelParam["topK"].GetInt();

    }
    return res;
}

} // end of namespace aif
