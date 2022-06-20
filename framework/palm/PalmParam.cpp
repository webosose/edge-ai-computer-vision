/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/palm/PalmParam.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <iostream>
#include <sstream>
#include <algorithm>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

PalmParam::PalmParam()
    : strides{8, 16, 16, 16}
    , optAspectRatios{1.0f}
    , interpolatedScaleAspectRatio(1.0f)
    , anchorOffsetX(0.5)
    , anchorOffsetY(0.5)
    , minScale(0.1484375)
    , maxScale(0.75)
    , reduceBoxesInLowestLayer(false)
    , scoreThreshold(0.5f)
    , iouThreshold(0.2f)
    , maxOutputSize(100)
    , updateThreshold(0.3f)
{
}

PalmParam::~PalmParam()
{
}

PalmParam::PalmParam(const PalmParam& other)
    : strides{other.strides}
    , optAspectRatios{other.optAspectRatios}
    , interpolatedScaleAspectRatio{other.interpolatedScaleAspectRatio}
    , anchorOffsetX{other.anchorOffsetX}
    , anchorOffsetY{other.anchorOffsetY}
    , minScale{other.minScale}
    , maxScale{other.maxScale}
    , reduceBoxesInLowestLayer{other.reduceBoxesInLowestLayer}
    , scoreThreshold{other.scoreThreshold}
    , iouThreshold{other.iouThreshold}
    , maxOutputSize{other.maxOutputSize}
    , updateThreshold{other.updateThreshold}
{
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}


PalmParam::PalmParam(PalmParam&& other) noexcept
    : strides{std::move(other.strides)}
    , optAspectRatios{std::move(other.optAspectRatios)}
    , interpolatedScaleAspectRatio{std::exchange(other.interpolatedScaleAspectRatio, 1.0f)}
    , anchorOffsetX{std::exchange(other.anchorOffsetX, 0.5)}
    , anchorOffsetY{std::exchange(other.anchorOffsetY, 0.5)}
    , minScale{std::exchange(other.minScale, 0.1484375)}
    , maxScale{std::exchange(other.maxScale, 0.75)}
    , reduceBoxesInLowestLayer{std::exchange(other.reduceBoxesInLowestLayer, false)}
    , scoreThreshold{std::exchange(other.scoreThreshold, 0.5f)}
    , iouThreshold{std::exchange(other.iouThreshold, 0.2f)}
    , maxOutputSize{std::exchange(other.maxOutputSize, 100)}
    , updateThreshold{std::exchange(other.updateThreshold, 0.3f)}
{
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

PalmParam& PalmParam::operator=(const PalmParam& other)
{
    // TRACE(TAG, "ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }

    strides = other.strides;
    optAspectRatios = other.optAspectRatios;
    interpolatedScaleAspectRatio = other.interpolatedScaleAspectRatio;
    anchorOffsetX = other.anchorOffsetX;
    anchorOffsetY = other.anchorOffsetY;
    minScale = other.minScale;
    maxScale = other.maxScale;
    reduceBoxesInLowestLayer = other.reduceBoxesInLowestLayer;
    scoreThreshold = other.scoreThreshold;
    iouThreshold = other.iouThreshold;
    maxOutputSize = other.maxOutputSize;
    updateThreshold = other.updateThreshold;

    return *this;
}

PalmParam& PalmParam::operator=(PalmParam&& other) noexcept
{
    // TRACE(TAG, "MOVE ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }
    strides = std::move(other.strides);
    optAspectRatios = std::move(other.optAspectRatios);
    interpolatedScaleAspectRatio = std::exchange(other.interpolatedScaleAspectRatio, 1.0f);
    anchorOffsetX = std::exchange(other.anchorOffsetX, 0.5);
    anchorOffsetY = std::exchange(other.anchorOffsetY, 0.5);
    minScale = std::exchange(other.minScale, 0.1484375);
    maxScale = std::exchange(other.maxScale, 0.75);
    reduceBoxesInLowestLayer = std::exchange(other.reduceBoxesInLowestLayer, false);
    scoreThreshold = std::exchange(other.scoreThreshold, 0.5f);
    iouThreshold = std::exchange(other.iouThreshold, 0.2f);
    maxOutputSize = std::exchange(other.maxOutputSize, 100);
    updateThreshold = std::exchange(other.updateThreshold, 0.3f);


    return *this;
}

bool PalmParam::operator==(const PalmParam& other) const
{
    return (
        (std::equal(strides.begin(), strides.end(), other.strides.begin())) &&
        (std::equal(optAspectRatios.begin(), optAspectRatios.end(), other.optAspectRatios.begin())) &&
        (std::abs(interpolatedScaleAspectRatio - other.interpolatedScaleAspectRatio) < aif::EPSILON) &&
        (std::abs(anchorOffsetX - other.anchorOffsetX) < aif::EPSILON) &&
        (std::abs(anchorOffsetY - other.anchorOffsetY) < aif::EPSILON) &&
        (std::abs(minScale - other.minScale) < aif::EPSILON) &&
        (std::abs(maxScale - other.maxScale) < aif::EPSILON) &&
        (reduceBoxesInLowestLayer == other.reduceBoxesInLowestLayer) &&
        (std::abs(scoreThreshold - other.scoreThreshold) < aif::EPSILON) &&
        (std::abs(iouThreshold - other.iouThreshold) < aif::EPSILON) &&
        (maxOutputSize == other.maxOutputSize) &&
        (std::abs(updateThreshold - other.updateThreshold) < aif::EPSILON)
    );
}

bool PalmParam::operator!=(const PalmParam& other) const
{
    return !operator==(other);
}

// debug
std::ostream& operator<<(std::ostream& os, const PalmParam& fp)
{
    os << "\n{\n";
    os << "\tstrides: [";
    for (int i = 0; i < fp.strides.size(); i++) {
        if (i == 0) {
            os << fp.strides[i];
        } else {
            os << ", " << fp.strides[i];
        }
    }
    os << "],\n";
    os << "\toptAspectRatio: [";
    for (int i = 0; i < fp.optAspectRatios.size(); i++) {
        if (i == 0) {
            os << fp.optAspectRatios[i];
        } else {
            os << ", " << fp.optAspectRatios[i];
        }
    }
    os << "],\n";
    os << "\tinterpolatedScaleAspectRatio: " << fp.interpolatedScaleAspectRatio << ",\n";
    os << "\tanchorOffsetX: " << fp.anchorOffsetX << ",\n";
    os << "\tanchorOffsetY: " << fp.anchorOffsetY << ",\n";
    os << "\tminScale: " << fp.minScale << ",\n";
    os << "\tmaxScale: " << fp.maxScale << ",\n";
    os << "\treduceBoxesInLowestLayer: " << (fp.reduceBoxesInLowestLayer ? "true" : "false") << ",\n";
    os << "\tscoreThreshold: " << fp.scoreThreshold << ",\n";
    os << "\tiouThreshold: " << fp.iouThreshold << ",\n";
    os << "\tmaxOutputSize: " << fp.maxOutputSize << "\n";
    os << "\tupdateThreshold: " << fp.updateThreshold << "\n";
    os << "}";

    return os;
}

void PalmParam::trace()
{
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

t_aif_status PalmParam::fromJson(const std::string& param)
{
    t_aif_status res = DetectorParam::fromJson(param);
    rj::Document payload;
    payload.Parse(param.c_str());
    if (payload.HasMember("modelParam")) {
        rj::Value& modelParam = payload["modelParam"];
        if (modelParam.HasMember("strides")) {
            strides.clear();
            for (auto& stride : modelParam["strides"].GetArray()) {
                strides.push_back(stride.GetInt());
            }
        }

        if (modelParam.HasMember("optAspectRatios")) {
            optAspectRatios.clear();
            for (auto& optAspectRatio : modelParam["optAspectRatios"].GetArray()) {
                optAspectRatios.push_back(optAspectRatio.GetFloat());
            }
        }

        if (modelParam.HasMember("interpolatedScaleAspectRatio"))
            interpolatedScaleAspectRatio = modelParam["interpolatedScaleAspectRatio"].GetFloat();

        if (modelParam.HasMember("anchorOffsetX"))
            anchorOffsetX = modelParam["anchorOffsetX"].GetFloat();

        if (modelParam.HasMember("anchorOffsetY"))
            anchorOffsetY = modelParam["anchorOffsetY"].GetFloat();

        if (modelParam.HasMember("minScale"))
            minScale = modelParam["minScale"].GetFloat();

        if (modelParam.HasMember("maxScale"))
            maxScale = modelParam["maxScale"].GetFloat();

        if (modelParam.HasMember("reduceBoxesInLowestLayer"))
            reduceBoxesInLowestLayer = modelParam["reduceBoxesInLowestLayer"].GetBool();

        if (modelParam.HasMember("scoreThreshold"))
            scoreThreshold = modelParam["scoreThreshold"].GetFloat();

        if (modelParam.HasMember("iouThreshold"))
            iouThreshold = modelParam["iouThreshold"].GetFloat();

        if (modelParam.HasMember("maxOutputSize"))
            maxOutputSize = modelParam["maxOutputSize"].GetInt();

        if (modelParam.HasMember("updateThreshold"))
            updateThreshold = modelParam["updateThreshold"].GetFloat();
    }
    return res;
}

} // end of namespace aif
