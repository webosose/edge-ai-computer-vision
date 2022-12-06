/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/personDetect/yolov3/Yolov3Param.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <iostream>
#include <sstream>
#include <algorithm>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

Yolov3Param::Yolov3Param()
    : detectObject("body")
    , origImgRoiX(0)
    , origImgRoiY(0)
    , origImgRoiWidth(0)
    , origImgRoiHeight(0)
    , lbbox_h(9)
    , lbbox_w(15)
    , mbbox_h(18)
    , mbbox_w(30)
    , box_size(3)
    , class_size(10)
    , stride{32, 16}
    , thresh_score{0, 24000, 35000, 58981}
    , thresh_iou_sc_nms(115)
    , thresh_iou_sc_sur(200)
    , thresh_iou_sc_avg(128)
    , thresh_iou_update(0.7)
{
}

Yolov3Param::~Yolov3Param()
{
}

Yolov3Param::Yolov3Param(const Yolov3Param& other)
    : detectObject(other.detectObject)
    , origImgRoiX(other.origImgRoiX)
    , origImgRoiY(other.origImgRoiY)
    , origImgRoiWidth(other.origImgRoiWidth)
    , origImgRoiHeight(other.origImgRoiHeight)
    , lbbox_h(other.lbbox_h)
    , lbbox_w(other.lbbox_w)
    , mbbox_h(other.mbbox_h)
    , mbbox_w(other.mbbox_w)
    , box_size(other.box_size)
    , class_size(other.class_size)
    , stride(other.stride)
    , thresh_score(other.thresh_score)
    , thresh_iou_sc_nms(other.thresh_iou_sc_nms)
    , thresh_iou_sc_sur(other.thresh_iou_sc_sur)
    , thresh_iou_sc_avg(other.thresh_iou_sc_avg)
    , thresh_iou_update(other.thresh_iou_update)
{
    // TRACE(TAG, "COPY CONSTRUCTOR....");
}

Yolov3Param::Yolov3Param(Yolov3Param&& other) noexcept
    : detectObject(std::move(other.detectObject))
    , origImgRoiX(std::move(other.origImgRoiX))
    , origImgRoiY(std::move(other.origImgRoiY))
    , origImgRoiWidth(std::move(other.origImgRoiWidth))
    , origImgRoiHeight(std::move(other.origImgRoiHeight))
    , lbbox_h(std::move(other.lbbox_h))
    , lbbox_w(std::move(other.lbbox_w))
    , mbbox_h(std::move(other.mbbox_h))
    , mbbox_w(std::move(other.mbbox_w))
    , box_size(std::move(other.box_size))
    , class_size(std::move(other.class_size))
    , stride(std::move(other.stride))
    , thresh_score(std::move(other.thresh_score))
    , thresh_iou_sc_nms(std::move(other.thresh_iou_sc_nms))
    , thresh_iou_sc_sur(std::move(other.thresh_iou_sc_sur))
    , thresh_iou_sc_avg(std::move(other.thresh_iou_sc_avg))
    , thresh_iou_update(std::move(other.thresh_iou_update))
{
    // TRACE(TAG, "MOVE CONSTRUCTOR....");
}

Yolov3Param& Yolov3Param::operator=(const Yolov3Param& other)
{
    // TRACE(TAG, "ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }

    detectObject = other.detectObject;
    origImgRoiX = other.origImgRoiX;
    origImgRoiY = other.origImgRoiY;
    origImgRoiWidth = other.origImgRoiWidth;
    origImgRoiHeight = other.origImgRoiHeight;
    lbbox_h = other.lbbox_h;
    lbbox_w = other.lbbox_w;
    mbbox_h = other.mbbox_h;
    mbbox_w = other.mbbox_w;
    box_size = other.box_size;
    class_size = other.class_size;
    stride = other.stride;
    thresh_score = other.thresh_score;
    thresh_iou_sc_nms = other.thresh_iou_sc_nms;
    thresh_iou_sc_sur = other.thresh_iou_sc_sur;
    thresh_iou_sc_avg = other.thresh_iou_sc_avg;
    thresh_iou_update = other.thresh_iou_update;

    return *this;
}

Yolov3Param& Yolov3Param::operator=(Yolov3Param&& other) noexcept
{
    // TRACE(TAG, "MOVE ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }

    detectObject = std::move(other.detectObject);
    origImgRoiX = std::move(other.origImgRoiX);
    origImgRoiY = std::move(other.origImgRoiY);
    origImgRoiWidth = std::move(other.origImgRoiWidth);
    origImgRoiHeight = std::move(other.origImgRoiHeight);
    lbbox_h = std::move(other.lbbox_h);
    lbbox_w = std::move(other.lbbox_w);
    mbbox_h = std::move(other.mbbox_h);
    mbbox_w = std::move(other.mbbox_w);
    box_size = std::move(other.box_size);
    class_size = std::move(other.class_size);
    stride = std::move(other.stride);
    thresh_score = std::move(other.thresh_score);
    thresh_iou_sc_nms = std::move(other.thresh_iou_sc_nms);
    thresh_iou_sc_sur = std::move(other.thresh_iou_sc_sur);
    thresh_iou_sc_avg = std::move(other.thresh_iou_sc_avg);
    thresh_iou_update = std::move(other.thresh_iou_update);

    return *this;
}

bool Yolov3Param::operator==(const Yolov3Param& other) const
{
    return (
        (detectObject == other.detectObject) &&
        (origImgRoiX == other.origImgRoiX) &&
        (origImgRoiY == other.origImgRoiY) &&
        (origImgRoiWidth == other.origImgRoiWidth) &&
        (origImgRoiHeight == other.origImgRoiHeight) &&
        (lbbox_h == other.lbbox_h) &&
        (lbbox_w == other.lbbox_w) &&
        (mbbox_h == other.mbbox_h) &&
        (mbbox_w == other.mbbox_w) &&
        (box_size == other.box_size) &&
        (class_size == other.class_size) &&
        (std::equal(stride.begin(), stride.end(), other.stride.begin())) &&
        (std::equal(thresh_score.begin(), thresh_score.end(), other.thresh_score.begin())) &&
        (thresh_iou_sc_nms == other.thresh_iou_sc_nms) &&
        (thresh_iou_sc_sur == other.thresh_iou_sc_sur) &&
        (thresh_iou_sc_avg == other.thresh_iou_sc_avg) &&
        (thresh_iou_update == other.thresh_iou_update)
    );
}

bool Yolov3Param::operator!=(const Yolov3Param& other) const
{
    return !operator==(other);
}

// debug
std::ostream& operator<<(std::ostream& os, const Yolov3Param& fp)
{
    os << "\n{\n";
    os << "\tdetectObject: " << fp.detectObject << ",\n";
    os << "\torigImgRoiX: " << fp.origImgRoiX << ",\n";
    os << "\torigImgRoiY: " << fp.origImgRoiY << ",\n";
    os << "\torigImgRoiWidth: " << fp.origImgRoiWidth << ",\n";
    os << "\torigImgRoiHeight: " << fp.origImgRoiHeight << ",\n";
    os << "\tlbbox_h: " << fp.lbbox_h << ",\n";
    os << "\tlbbox_w: " << fp.lbbox_w << ",\n";
    os << "\tmbbox_h: " << fp.mbbox_h << ",\n";
    os << "\tmbbox_w: " << fp.mbbox_w << ",\n";
    os << "\tbox_size: " << fp.box_size << ",\n";
    os << "\tclass_size: " << fp.class_size << ",\n";
    os << "\tstride: [";
    for (int i = 0; i < fp.stride.size(); i++) {
        if (i == 0) {
            os << fp.stride[i];
        } else {
            os << ", " << fp.stride[i];
        }
    }
    os << "],\n";
    os << "\tthresh_score: [";
    for (int i = 0; i < fp.thresh_score.size(); i++) {
        if (i == 0) {
            os << fp.thresh_score[i];
        } else {
            os << ", " << fp.thresh_score[i];
        }
    }
    os << "],\n";
    os << "\tthresh_iou_sc_nms: " << fp.thresh_iou_sc_nms << ",\n";
    os << "\tthresh_iou_sc_sur: " << fp.thresh_iou_sc_sur << ",\n";
    os << "\tthresh_iou_sc_avg: " << fp.thresh_iou_sc_avg << ",\n";
    os << "\tthresh_iou_update: " << fp.thresh_iou_update << ",\n";

    os << "}";
    return os;
}

void Yolov3Param::trace()
{
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

t_aif_status Yolov3Param::fromJson(const std::string& param)
{
    t_aif_status res = BodyPoseEstimationParam::fromJson(param);
    rj::Document payload;
    payload.Parse(param.c_str());
    if (payload.HasMember("modelParam")) {
        rj::Value& modelParam = payload["modelParam"];
        if (modelParam.HasMember("detectObject")) {
            detectObject = modelParam["detectObject"].GetString();
        }
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
        if (modelParam.HasMember("lbbox_h")) {
            lbbox_h = modelParam["lbbox_h"].GetInt();
        }
        if (modelParam.HasMember("lbbox_w")) {
            lbbox_w = modelParam["lbbox_w"].GetInt();
        }
        if (modelParam.HasMember("mbbox_h")) {
            mbbox_h = modelParam["mbbox_h"].GetInt();
        }
        if (modelParam.HasMember("mbbox_w")) {
            mbbox_w = modelParam["mbbox_w"].GetInt();
        }
        if (modelParam.HasMember("box_size")) {
            box_size = modelParam["box_size"].GetInt();
        }
        if (modelParam.HasMember("class_size")) {
            class_size = modelParam["class_size"].GetInt();
        }
        if (modelParam.HasMember("stride")) {
            stride.clear();
            for (auto& str : modelParam["stride"].GetArray()) {
                stride.push_back(str.GetInt());
            }
        }
        if (modelParam.HasMember("thresh_score")) {
            thresh_score.clear();
            for (auto& th_score : modelParam["thresh_score"].GetArray()) {
                thresh_score.push_back(th_score.GetInt());
            }
        }
        if (modelParam.HasMember("thresh_iou_sc_nms")) {
            thresh_iou_sc_nms = modelParam["thresh_iou_sc_nms"].GetInt();
        }
        if (modelParam.HasMember("thresh_iou_sc_sur")) {
            thresh_iou_sc_sur = modelParam["thresh_iou_sc_sur"].GetInt();
        }
        if (modelParam.HasMember("thresh_iou_sc_avg")) {
            thresh_iou_sc_avg = modelParam["thresh_iou_sc_avg"].GetInt();
        }
        if (modelParam.HasMember("thresh_iou_update")) {
            thresh_iou_update = modelParam["thresh_iou_update"].GetFloat();
        }
    }

    return res;
}

}
