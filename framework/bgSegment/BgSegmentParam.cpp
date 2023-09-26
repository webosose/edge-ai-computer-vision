/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/bgSegment/BgSegmentParam.h>
#include <aif/tools/Utils.h>

#include <algorithm>
#include <iostream>
#include <sstream>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

BgSegmentParam::BgSegmentParam()
    : origImgRoiX(0)
    , origImgRoiY(0)
    , origImgRoiWidth(0)
    , origImgRoiHeight(0)
{
}

BgSegmentParam::~BgSegmentParam() {}

BgSegmentParam::BgSegmentParam(const BgSegmentParam &other)
    : origImgRoiX(other.origImgRoiX)
    , origImgRoiY(other.origImgRoiY)
    , origImgRoiWidth(other.origImgRoiWidth)
    , origImgRoiHeight(other.origImgRoiHeight)
{
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

BgSegmentParam::BgSegmentParam(BgSegmentParam &&other) noexcept
    : origImgRoiX(std::move(other.origImgRoiX))
    , origImgRoiY(std::move(other.origImgRoiY))
    , origImgRoiWidth(std::move(other.origImgRoiWidth))
    , origImgRoiHeight(std::move(other.origImgRoiHeight))
{
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

BgSegmentParam& BgSegmentParam::operator=(const BgSegmentParam& other)
{
    // TRACE(TAG, "ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }

    origImgRoiX = other.origImgRoiX;
    origImgRoiY = other.origImgRoiY;
    origImgRoiWidth = other.origImgRoiWidth;
    origImgRoiHeight = other.origImgRoiHeight;
    return *this;
}

BgSegmentParam& BgSegmentParam::operator=(BgSegmentParam&& other) noexcept
{
    // TRACE(TAG, "MOVE ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }

    origImgRoiX = std::move(other.origImgRoiX);
    origImgRoiY = std::move(other.origImgRoiY);
    origImgRoiWidth = std::move(other.origImgRoiWidth);
    origImgRoiHeight = std::move(other.origImgRoiHeight);
    return *this;
}

bool BgSegmentParam::operator==(const BgSegmentParam &other) const {
    return (
        (origImgRoiX == other.origImgRoiX) &&
        (origImgRoiY == other.origImgRoiY) &&
        (origImgRoiWidth == other.origImgRoiWidth) &&
        (origImgRoiHeight == other.origImgRoiHeight));
}

bool BgSegmentParam::operator!=(const BgSegmentParam &other) const {
    return !operator==(other);
}

// debug
std::ostream &operator<<(std::ostream &os, const BgSegmentParam &fp) {
    os << "\n{\n";
    os << "\torigImgRoiXX: " << fp.origImgRoiX << ",\n";
    os << "\torigImgRoiY: " << fp.origImgRoiY << ",\n";
    os << "\torigImgRoiWidth: " << fp.origImgRoiWidth << ",\n";
    os << "\torigImgRoiHeight: " << fp.origImgRoiHeight << ",\n";

    os << "}";
    return os;
}

void BgSegmentParam::trace() {
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

t_aif_status BgSegmentParam::fromJson(const std::string& param)
{
    t_aif_status res = DetectorParam::fromJson(param);
    rj::Document payload;
    payload.Parse(param.c_str());
    if (payload.HasMember("modelParam")) {
        rj::Value& modelParam = payload["modelParam"];
        if (modelParam.HasMember("origImgRoiX")) {
            origImgRoiX = modelParam["origImgRoiX"].GetInt();
        }
        if (modelParam.HasMember("origImgRoiY")) {
            origImgRoiY = modelParam["origImgRoiY"].GetInt();
        }
        if (modelParam.HasMember("origImgRoiWidth")) {
            origImgRoiWidth = modelParam["origImgRoiWidth"].GetInt();
        }
        if (modelParam.HasMember("origImgRoiHeight")) {
            origImgRoiHeight = modelParam["origImgRoiHeight"].GetInt();
        }
    }

    return res;
}

} // namespace aif
