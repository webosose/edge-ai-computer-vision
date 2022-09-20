/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/BodyPoseEstimationParam.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <iostream>
#include <sstream>
#include <algorithm>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

BodyPoseEstimationParam::BodyPoseEstimationParam()
    : orgImgWidth(640)
    , orgImgHeight(480)
    , numMaxPerson(1)
    , hasIntrinsics(true)
    , timeLogIntervalMs(0)
{
}

BodyPoseEstimationParam::~BodyPoseEstimationParam()
{
}

BodyPoseEstimationParam::BodyPoseEstimationParam(const BodyPoseEstimationParam& other)
    : orgImgWidth(other.orgImgWidth)
    , orgImgHeight(other.orgImgHeight)
    , numMaxPerson(other.numMaxPerson)
    , hasIntrinsics(other.hasIntrinsics)
    , timeLogIntervalMs(other.timeLogIntervalMs)
{
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

BodyPoseEstimationParam::BodyPoseEstimationParam(BodyPoseEstimationParam&& other) noexcept
    : orgImgWidth(std::move(other.orgImgWidth))
    , orgImgHeight(std::move(other.orgImgHeight))
    , numMaxPerson(std::move(other.numMaxPerson))
    , hasIntrinsics(std::move(other.hasIntrinsics))
    , timeLogIntervalMs(std::move(other.timeLogIntervalMs))
{
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

BodyPoseEstimationParam& BodyPoseEstimationParam::operator=(const BodyPoseEstimationParam& other)
{
    // TRACE(TAG, "ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }

    orgImgWidth = other.orgImgWidth;
    orgImgHeight = other.orgImgHeight;
    numMaxPerson = other.numMaxPerson;
    hasIntrinsics = other.hasIntrinsics;
    timeLogIntervalMs = other.timeLogIntervalMs;

    return *this;
}

BodyPoseEstimationParam& BodyPoseEstimationParam::operator=(BodyPoseEstimationParam&& other) noexcept
{
    // TRACE(TAG, "MOVE ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }

    orgImgWidth = std::move(other.orgImgWidth);
    orgImgHeight = std::move(other.orgImgHeight);
    numMaxPerson = std::move(other.numMaxPerson);
    hasIntrinsics = std::move(other.hasIntrinsics);
    timeLogIntervalMs = std::move(other.timeLogIntervalMs);

    return *this;
}

bool BodyPoseEstimationParam::operator==(const BodyPoseEstimationParam& other) const
{
    return (
        (orgImgWidth == other.orgImgWidth) &&
        (orgImgHeight == other.orgImgHeight) &&
        (numMaxPerson == other.numMaxPerson) &&
        (hasIntrinsics == other.hasIntrinsics) &&
        (timeLogIntervalMs == other.timeLogIntervalMs)
    );
}

bool BodyPoseEstimationParam::operator!=(const BodyPoseEstimationParam& other) const
{
    return !operator==(other);
}

// debug
std::ostream& operator<<(std::ostream& os, const BodyPoseEstimationParam& bp)
{
    os << "\n{\n";
    os << "\torgImgWidth: " << bp.orgImgWidth << ",\n";
    os << "\torgImgHeight: " << bp.orgImgHeight << ",\n";
    os << "\tnumMaxPerson: " << bp.numMaxPerson << ",\n";
    os << "\thasIntrinsics: " << bp.hasIntrinsics << ",\n";
    os << "\ttimeLogIntervalMs: " << bp.timeLogIntervalMs << ",\n";
    os << "}";
    return os;
}

void BodyPoseEstimationParam::trace()
{
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

t_aif_status BodyPoseEstimationParam::fromJson(const std::string& param)
{
    t_aif_status res = DetectorParam::fromJson(param);
    rj::Document payload;
    payload.Parse(param.c_str());
    if (payload.HasMember("commonParam")) {
        rj::Value& commonParam = payload["commonParam"];
        if (commonParam.HasMember("orgImgWidth")) {
            orgImgWidth = commonParam["orgImgWidth"].GetInt();
        }
        if (commonParam.HasMember("orgImgHeight")) {
            orgImgHeight = commonParam["orgImgHeight"].GetInt();
        }
        if (commonParam.HasMember("numMaxPerson")) {
            numMaxPerson = commonParam["numMaxPerson"].GetInt();
        }
        if (commonParam.HasMember("hasIntrinsics")) {
            hasIntrinsics = commonParam["hasIntrinsics"].GetBool();
        }
        if (commonParam.HasMember("timeLogIntervalsMs")) {
            timeLogIntervalMs = commonParam["timeLogIntervalsMs"].GetInt();
        }
    }

    return res;
}

}
