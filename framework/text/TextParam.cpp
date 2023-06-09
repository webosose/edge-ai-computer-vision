/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/text/TextParam.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <iostream>
#include <sstream>
#include <algorithm>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

TextParam::TextParam()
    : m_boxThreshold(0.7f)
    , m_useDetectionRegion(false)
{
}

TextParam::~TextParam()
{
}

TextParam::TextParam(const TextParam& other)
    : m_boxThreshold(other.m_boxThreshold)
    , m_useDetectionRegion(other.m_useDetectionRegion)
    , m_detectionRegion(other.m_detectionRegion)
{
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

TextParam::TextParam(TextParam&& other) noexcept
    : m_boxThreshold(other.m_boxThreshold)
    , m_useDetectionRegion(other.m_useDetectionRegion)
    , m_detectionRegion(other.m_detectionRegion)
{
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

TextParam& TextParam::operator=(const TextParam& other)
{
    // TRACE(TAG, "ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }
    m_boxThreshold = other.m_boxThreshold;
    m_useDetectionRegion = other.m_useDetectionRegion;
    m_detectionRegion = other.m_detectionRegion;

    return *this;
}

TextParam& TextParam::operator=(TextParam&& other) noexcept
{
    // TRACE(TAG, "MOVE ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }

    m_boxThreshold = std::move(other.m_boxThreshold);
    m_useDetectionRegion = std::move(other.m_useDetectionRegion);
    m_detectionRegion = std::move(other.m_detectionRegion);
    
    return *this;
}

bool TextParam::operator==(const TextParam& other) const
{
    return
        (std::abs(m_boxThreshold - other.m_boxThreshold) < aif::EPSILON);
}

bool TextParam::operator!=(const TextParam& other) const
{
    return !operator==(other);
}

// debug
std::ostream& operator<<(std::ostream& os, const TextParam& fp)
{
    os << "\n{\n";
    os << "\tboxThreshold: " << fp.m_boxThreshold;
    os << "\tuseDetectionRegion: " << fp.m_useDetectionRegion;
    os << "\tdetectionRegion: " << fp.m_detectionRegion << "\n";
    os << "}";
    return os;
}

void TextParam::trace()
{
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

t_aif_status TextParam::fromJson(const std::string& param)
{
    t_aif_status res = DetectorParam::fromJson(param);
    rj::Document payload;
    payload.Parse(param.c_str());
    if (payload.HasMember("modelParam")) {
        rj::Value& modelParam = payload["modelParam"];
        if (modelParam.HasMember("boxThreshold")) {
            m_boxThreshold = modelParam["boxThreshold"].GetFloat();
            Logi("set boxThreshold : ", m_boxThreshold);
        }
        if (modelParam.HasMember("detectionRegion") &&
            modelParam["detectionRegion"].Size()  == 4) {
            m_useDetectionRegion = true;
            m_detectionRegion = cv::Rect(
                modelParam["detectionRegion"][0].GetInt(),
                modelParam["detectionRegion"][1].GetInt(),
                modelParam["detectionRegion"][2].GetInt(),
                modelParam["detectionRegion"][3].GetInt()
            );
            Logi("set detection region: ");
            Logi(" x : ", m_detectionRegion.x);
            Logi(" y : ", m_detectionRegion.y);
            Logi(" width : ", m_detectionRegion.width);
            Logi(" height: ", m_detectionRegion.height);
        }
    }
    return res;
}

}  // end of namespace aif
