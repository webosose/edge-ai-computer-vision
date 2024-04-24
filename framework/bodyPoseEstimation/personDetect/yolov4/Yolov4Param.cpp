/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/personDetect/yolov4/Yolov4Param.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <iostream>
#include <sstream>
#include <algorithm>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

Yolov4Param::Yolov4Param()
    : origImgRoiX(0)
    , origImgRoiY(0)
    , origImgRoiWidth(0)
    , origImgRoiHeight(0)
    , bboxBottomThresholdY(-1)
    , useFp16(false)
    , nms_threshold(0.5)
    , bbox_conf_threshold(0.1)
    , thresh_confidence(-1.0)
    , numClasses(80)     /* from yolo4_tiny_relu.json */
    , numOutChannels(85) /* numClasses + 5 : x, y, w, h, score + classes */
    , strides{32, 16}
    , numAnchors{3, 3}
    , anchors{
           {
             {81, 82},
             {135, 169},
             {344, 319},
           },
           {
             {23, 27},
             {37, 58},
             {81, 82},
           }
         }
{
    for (int i=0; i<strides.size(); i++) {
        auto stride = strides[i];
        auto& anchors_ = anchors[i];
        std::vector<std::pair<float, float>> aNorm;

        for (const auto& a : anchors_) {
            aNorm.push_back( std::make_pair(
                             static_cast<float>(a.first) / stride,
                             static_cast<float>(a.second) / stride ) );
        }
        anchorsNorm.push_back( aNorm );
    }

}

Yolov4Param::~Yolov4Param()
{
}

Yolov4Param::Yolov4Param(const Yolov4Param& other)
    : origImgRoiX(other.origImgRoiX)
    , origImgRoiY(other.origImgRoiY)
    , origImgRoiWidth(other.origImgRoiWidth)
    , origImgRoiHeight(other.origImgRoiHeight)
    , bboxBottomThresholdY(other.bboxBottomThresholdY)
    , useFp16(other.useFp16)
    , nms_threshold(other.nms_threshold)
    , bbox_conf_threshold(other.bbox_conf_threshold)
    , thresh_confidence(other.thresh_confidence)
    , numClasses(other.numClasses)
    , numOutChannels(other.numOutChannels)
    , strides(other.strides)
    , numAnchors(other.numAnchors)
    , anchors(other.anchors)
    , anchorsNorm(other.anchorsNorm)
{
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

Yolov4Param::Yolov4Param(Yolov4Param&& other) noexcept
    : origImgRoiX(std::move(other.origImgRoiX))
    , origImgRoiY(std::move(other.origImgRoiY))
    , origImgRoiWidth(std::move(other.origImgRoiWidth))
    , origImgRoiHeight(std::move(other.origImgRoiHeight))
    , bboxBottomThresholdY(std::move(other.bboxBottomThresholdY))
    , useFp16(std::move(other.useFp16))
    , nms_threshold(std::move(other.nms_threshold))
    , bbox_conf_threshold(std::move(other.bbox_conf_threshold))
    , thresh_confidence(std::move(other.thresh_confidence))
    , numClasses(std::move(other.numClasses))
    , numOutChannels(std::move(other.numOutChannels))
    , strides(std::move(other.strides))
    , numAnchors(std::move(other.numAnchors))
    , anchors(std::move(other.anchors))
    , anchorsNorm(std::move(other.anchorsNorm))
{
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

Yolov4Param& Yolov4Param::operator=(const Yolov4Param& other)
{
    // TRACE(TAG, "ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }

    origImgRoiX = other.origImgRoiX;
    origImgRoiY = other.origImgRoiY;
    origImgRoiWidth = other.origImgRoiWidth;
    origImgRoiHeight = other.origImgRoiHeight;
    bboxBottomThresholdY = other.bboxBottomThresholdY;
    useFp16 = other.useFp16;
    nms_threshold = other.nms_threshold;
    bbox_conf_threshold = other.bbox_conf_threshold;
    thresh_confidence = other.thresh_confidence;
    numClasses = other.numClasses;
    numOutChannels = other.numOutChannels;
    strides = other.strides;
    numAnchors = other.numAnchors;
    anchors = other.anchors;
    anchorsNorm = other.anchorsNorm;

    return *this;
}

Yolov4Param& Yolov4Param::operator=(Yolov4Param&& other) noexcept
{
    // TRACE(TAG, "MOVE ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }

    origImgRoiX = std::move(other.origImgRoiX);
    origImgRoiY = std::move(other.origImgRoiY);
    origImgRoiWidth = std::move(other.origImgRoiWidth);
    origImgRoiHeight = std::move(other.origImgRoiHeight);
    bboxBottomThresholdY = std::move(other.bboxBottomThresholdY);
    useFp16 = std::move(other.useFp16);
    nms_threshold = std::move(other.nms_threshold);
    bbox_conf_threshold = std::move(other.bbox_conf_threshold);
    thresh_confidence = std::move(other.thresh_confidence);
    numClasses = std::move(other.numClasses);
    numOutChannels = std::move(other.numOutChannels);
    strides = std::move(other.strides);
    numAnchors = std::move(other.numAnchors);
    anchors = std::move(other.anchors);
    anchorsNorm = std::move(other.anchorsNorm);

    return *this;
}

bool Yolov4Param::operator==(const Yolov4Param& other) const
{
    return (
        (origImgRoiX == other.origImgRoiX) &&
        (origImgRoiY == other.origImgRoiY) &&
        (origImgRoiWidth == other.origImgRoiWidth) &&
        (origImgRoiHeight == other.origImgRoiHeight) &&
        (bboxBottomThresholdY == other.bboxBottomThresholdY) &&
        (useFp16 == other.useFp16) &&
        (std::abs(nms_threshold - other.nms_threshold) < aif::EPSILON) &&
        (std::abs(bbox_conf_threshold - other.bbox_conf_threshold) < aif::EPSILON) &&
        (std::abs(thresh_confidence - other.thresh_confidence) < aif::EPSILON) &&
        (numClasses == other.numClasses) &&
        (numOutChannels == other.numOutChannels) &&
        (std::equal(strides.begin(), strides.end(), other.strides.begin())) &&
        (std::equal(numAnchors.begin(), numAnchors.end(), other.numAnchors.begin())) &&
        (std::equal(anchors.begin(), anchors.end(), other.anchors.begin())) &&
        (std::equal(anchorsNorm.begin(), anchorsNorm.end(), other.anchorsNorm.begin()))
    );
}

bool Yolov4Param::operator!=(const Yolov4Param& other) const
{
    return !operator==(other);
}

// debug
std::ostream& operator<<(std::ostream& os, const Yolov4Param& fp)
{
    os << "\n{\n";
    os << "\torigImgRoiX: " << fp.origImgRoiX << ",\n";
    os << "\torigImgRoiY: " << fp.origImgRoiY << ",\n";
    os << "\torigImgRoiWidth: " << fp.origImgRoiWidth << ",\n";
    os << "\torigImgRoiHeight: " << fp.origImgRoiHeight << ",\n";
    os << "\tbboxBottomThresholdY: " << fp.bboxBottomThresholdY << ",\n";
    os << "\tuseFp16: " << fp.useFp16 << ",\n";
    os << "\tnms_threshold: " << fp.nms_threshold << ",\n";
    os << "\tbbox_conf_threshold: " << fp.bbox_conf_threshold << ",\n";
    os << "\tthresh_confidence: " << fp.thresh_confidence << ",\n";
    os << "\tnumClasses: " << fp.numClasses << ",\n";
    os << "\tnumOutChannels: " << fp.numOutChannels << ",\n";
    os << "\tstrides: [";
    for (int i = 0; i < fp.strides.size(); i++) {
        if (i == 0) {
            os << fp.strides[i];
        } else {
            os << ", " << fp.strides[i];
        }
    }
    os << "],\n";
    os << "\tnumAnchors: [";
    for (int i = 0; i < fp.numAnchors.size(); i++) {
        if (i == 0) {
            os << fp.numAnchors[i];
        } else {
            os << ", " << fp.numAnchors[i];
        }
    }
    os << "],\n";
    os << "\tanchors: [";
    for (int i = 0; i < fp.anchors.size(); i++) { // 2
        for (int j = 0; j < fp.numAnchors[i]; j++) { // 3
            os << "[" << fp.anchors[i][j].first << "," << fp.anchors[i][j].second << "]\n";
        }
    }
    os << "],\n";
    os << "\tanchorsNorm: [";
    for (int i = 0; i < fp.anchorsNorm.size(); i++) { // 2
        for (int j = 0; j < fp.numAnchors[i]; j++) { // 3
            os << "[" << fp.anchorsNorm[i][j].first << "," << fp.anchorsNorm[i][j].second << "]\n";
        }
    }
    os << "],\n";


    os << "}";
    return os;
}

void Yolov4Param::trace()
{
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

t_aif_status Yolov4Param::fromJson(const std::string& param)
{
    t_aif_status res = BodyPoseEstimationParam::fromJson(param);
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
        if (modelParam.HasMember("bboxBottomThresholdY")) {
            bboxBottomThresholdY = modelParam["bboxBottomThresholdY"].GetInt();
        }
        if (modelParam.HasMember("useFp16")) {
            useFp16 = modelParam["useFp16"].GetBool();
        }
        if (modelParam.HasMember("nms_threshold")) {
            nms_threshold = modelParam["nms_threshold"].GetDouble();
        }
        if (modelParam.HasMember("bbox_conf_threshold")) {
            bbox_conf_threshold = modelParam["bbox_conf_threshold"].GetDouble();
        }
        if (modelParam.HasMember("thresh_confidence")) {
            thresh_confidence = modelParam["thresh_confidence"].GetDouble();
        }
        if (modelParam.HasMember("numClasses")) {
            numClasses = modelParam["numClasses"].GetUint();
            numOutChannels = numClasses + 5;
        }
        if (modelParam.HasMember("strides")) {
            strides.clear();
            for (auto& stride : modelParam["strides"].GetArray()) {
                strides.push_back(stride.GetInt());
            }
        }
        if (modelParam.HasMember("numAnchors")) {
            numAnchors.clear();
            for (auto& numAnchor : modelParam["numAnchors"].GetArray()) {
                numAnchors.push_back(numAnchor.GetInt());
            }
        }
        if (modelParam.HasMember("anchors")) {
            anchors.clear();
            auto anchors1d = modelParam["anchors"].GetArray();
            auto offset = 0U;
            for (auto n : numAnchors) {
                std::vector<std::pair<int, int>> anchor_wh;
                while (n--) {
                    float anchor_w = anchors1d[offset].GetFloat();
                    float anchor_h = anchors1d[offset+1].GetFloat();
                    anchor_wh.push_back( std::make_pair(anchor_w, anchor_h) );
                    offset += 2;
                }
                anchors.push_back( anchor_wh );
            }
        }
    }

    return res;
}

}
