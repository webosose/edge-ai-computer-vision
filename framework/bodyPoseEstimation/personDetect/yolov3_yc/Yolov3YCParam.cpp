/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/personDetect/yolov3_yc/Yolov3YCParam.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <iostream>
#include <sstream>
#include <algorithm>

namespace {
static const char TAG[] = "<FPARAM>";
} // anonymous namespace

namespace aif {

Yolov3YCParam::Yolov3YCParam()
    : detectObject("body")
    , gt_bboxes()
    , origImgRoiX(0)
    , origImgRoiY(0)
    , origImgRoiWidth(0)
    , origImgRoiHeight(0)
    , lbbox_h(9)
    , lbbox_w(15)
    , mbbox_h(18)
    , mbbox_w(30)
    , sbbox_h(36)
    , sbbox_w(60)
    , box_size(3)
    , sb_box_size(1)
    , class_size(10)
    , stride{32, 16, 8}
    , thresh_score{0, 24000, 35000, 58981}
    , thresh_iou_sc_nms(115)
    , thresh_iou_sc_sur(200)
    , thresh_iou_sc_avg(128)
    , thresh_iou_update(0.7)
{
}

Yolov3YCParam::~Yolov3YCParam()
{
}

Yolov3YCParam::Yolov3YCParam(const Yolov3YCParam& other)
    : detectObject(other.detectObject)
    , gt_bboxes(other.gt_bboxes)
    , origImgRoiX(other.origImgRoiX)
    , origImgRoiY(other.origImgRoiY)
    , origImgRoiWidth(other.origImgRoiWidth)
    , origImgRoiHeight(other.origImgRoiHeight)
    , lbbox_h(other.lbbox_h)
    , lbbox_w(other.lbbox_w)
    , mbbox_h(other.mbbox_h)
    , mbbox_w(other.mbbox_w)
    , sbbox_h(other.sbbox_h)
    , sbbox_w(other.sbbox_w)
    , box_size(other.box_size)
    , sb_box_size(other.sb_box_size)
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

Yolov3YCParam::Yolov3YCParam(Yolov3YCParam&& other) noexcept
    : detectObject(std::move(other.detectObject))
    , gt_bboxes(std::move(other.gt_bboxes))
    , origImgRoiX(std::move(other.origImgRoiX))
    , origImgRoiY(std::move(other.origImgRoiY))
    , origImgRoiWidth(std::move(other.origImgRoiWidth))
    , origImgRoiHeight(std::move(other.origImgRoiHeight))
    , lbbox_h(std::move(other.lbbox_h))
    , lbbox_w(std::move(other.lbbox_w))
    , mbbox_h(std::move(other.mbbox_h))
    , mbbox_w(std::move(other.mbbox_w))
    , sbbox_h(std::move(other.sbbox_h))
    , sbbox_w(std::move(other.sbbox_w))
    , box_size(std::move(other.box_size))
    , sb_box_size(std::move(other.sb_box_size))
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

Yolov3YCParam& Yolov3YCParam::operator=(const Yolov3YCParam& other)
{
    // TRACE(TAG, "ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }

    detectObject = other.detectObject;
    gt_bboxes = other.gt_bboxes;
    origImgRoiX = other.origImgRoiX;
    origImgRoiY = other.origImgRoiY;
    origImgRoiWidth = other.origImgRoiWidth;
    origImgRoiHeight = other.origImgRoiHeight;
    lbbox_h = other.lbbox_h;
    lbbox_w = other.lbbox_w;
    mbbox_h = other.mbbox_h;
    mbbox_w = other.mbbox_w;
    sbbox_h = other.sbbox_h;
    sbbox_w = other.sbbox_w;
    box_size = other.box_size;
    sb_box_size = other.sb_box_size;
    class_size = other.class_size;
    stride = other.stride;
    thresh_score = other.thresh_score;
    thresh_iou_sc_nms = other.thresh_iou_sc_nms;
    thresh_iou_sc_sur = other.thresh_iou_sc_sur;
    thresh_iou_sc_avg = other.thresh_iou_sc_avg;
    thresh_iou_update = other.thresh_iou_update;

    return *this;
}

Yolov3YCParam& Yolov3YCParam::operator=(Yolov3YCParam&& other) noexcept
{
    // TRACE(TAG, "MOVE ASSIGNMENT OPERATOR....");
    if (this == &other) {
        return *this;
    }

    detectObject = std::move(other.detectObject);
    gt_bboxes = std::move(other.gt_bboxes);
    origImgRoiX = std::move(other.origImgRoiX);
    origImgRoiY = std::move(other.origImgRoiY);
    origImgRoiWidth = std::move(other.origImgRoiWidth);
    origImgRoiHeight = std::move(other.origImgRoiHeight);
    lbbox_h = std::move(other.lbbox_h);
    lbbox_w = std::move(other.lbbox_w);
    mbbox_h = std::move(other.mbbox_h);
    mbbox_w = std::move(other.mbbox_w);
    sbbox_h = std::move(other.sbbox_h);
    sbbox_w = std::move(other.sbbox_w);
    box_size = std::move(other.box_size);
    sb_box_size = std::move(other.sb_box_size);
    class_size = std::move(other.class_size);
    stride = std::move(other.stride);
    thresh_score = std::move(other.thresh_score);
    thresh_iou_sc_nms = std::move(other.thresh_iou_sc_nms);
    thresh_iou_sc_sur = std::move(other.thresh_iou_sc_sur);
    thresh_iou_sc_avg = std::move(other.thresh_iou_sc_avg);
    thresh_iou_update = std::move(other.thresh_iou_update);

    return *this;
}

bool Yolov3YCParam::operator==(const Yolov3YCParam& other) const
{
    return (
        (detectObject == other.detectObject) &&
        (gt_bboxes == other.gt_bboxes) &&
        (origImgRoiX == other.origImgRoiX) &&
        (origImgRoiY == other.origImgRoiY) &&
        (origImgRoiWidth == other.origImgRoiWidth) &&
        (origImgRoiHeight == other.origImgRoiHeight) &&
        (lbbox_h == other.lbbox_h) &&
        (lbbox_w == other.lbbox_w) &&
        (mbbox_h == other.mbbox_h) &&
        (mbbox_w == other.mbbox_w) &&
        (sbbox_h == other.sbbox_h) &&
        (sbbox_w == other.sbbox_w) &&
        (box_size == other.box_size) &&
        (sb_box_size == other.sb_box_size) &&
        (class_size == other.class_size) &&
        (std::equal(stride.begin(), stride.end(), other.stride.begin())) &&
        (std::equal(thresh_score.begin(), thresh_score.end(), other.thresh_score.begin())) &&
        (thresh_iou_sc_nms == other.thresh_iou_sc_nms) &&
        (thresh_iou_sc_sur == other.thresh_iou_sc_sur) &&
        (thresh_iou_sc_avg == other.thresh_iou_sc_avg) &&
        (floatEquals(thresh_iou_update, other.thresh_iou_update))
    );
}

bool Yolov3YCParam::operator!=(const Yolov3YCParam& other) const
{
    return !operator==(other);
}

// debug
std::ostream& operator<<(std::ostream& os, const Yolov3YCParam& fp)
{
    os << "\n{\n";
    os << "\tdetectObject: " << fp.detectObject << ",\n";
    os << "\tgt_bboxes: [";
    for (int i = 0; i < fp.gt_bboxes.size(); i++) {
        auto gt_bbox = fp.gt_bboxes[i]; // pair
        os << gt_bbox.first << ",\n";
        for (int j = 0; j < 4; j++) {
            if (j == 0) {
                os << gt_bbox.second[j];
            } else {
                os << ", " << gt_bbox.second[j];
            }
        }
    }
    os << "],\n";

    os << "\torigImgRoiX: " << fp.origImgRoiX << ",\n";
    os << "\torigImgRoiY: " << fp.origImgRoiY << ",\n";
    os << "\torigImgRoiWidth: " << fp.origImgRoiWidth << ",\n";
    os << "\torigImgRoiHeight: " << fp.origImgRoiHeight << ",\n";
    os << "\tlbbox_h: " << fp.lbbox_h << ",\n";
    os << "\tlbbox_w: " << fp.lbbox_w << ",\n";
    os << "\tmbbox_h: " << fp.mbbox_h << ",\n";
    os << "\tmbbox_w: " << fp.mbbox_w << ",\n";
    os << "\tsbbox_h: " << fp.sbbox_h << ",\n";
    os << "\tsbbox_w: " << fp.sbbox_w << ",\n";
    os << "\tbox_size: " << fp.box_size << ",\n";
    os << "\tsb_box_size: " << fp.sb_box_size << ",\n";
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

void Yolov3YCParam::trace()
{
    std::stringstream ss;
    ss << *this;
    TRACE(TAG, ss.str());
}

t_aif_status Yolov3YCParam::fromJson(const std::string& param)
{
    t_aif_status res = BodyPoseEstimationParam::fromJson(param);
    rj::Document payload;
    payload.Parse(param.c_str());
    if (payload.HasMember("modelParam")) {
        rj::Value& modelParam = payload["modelParam"];
        if (modelParam.HasMember("detectObject")) {
            detectObject = (modelParam["detectObject"].GetString() != nullptr ? modelParam["detectObject"].GetString() : "");
        }
        if (modelParam.HasMember("gt_bboxes")) {
            gt_bboxes.clear();
            for (auto& bbox : modelParam["gt_bboxes"].GetArray()) {
                std::string fname = bbox[0].GetString();
                std::vector<float> box;
                for (auto id = 1; id <= 4; id++) {
                    box.push_back(bbox[id].GetFloat());
                }
                gt_bboxes.push_back(std::make_pair(fname, box));
            }
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
        if (modelParam.HasMember("sbbox_h")) {
            sbbox_h = modelParam["sbbox_h"].GetInt();
        }
        if (modelParam.HasMember("sbbox_w")) {
            sbbox_w = modelParam["sbbox_w"].GetInt();
        }
        if (modelParam.HasMember("box_size")) {
            box_size = modelParam["box_size"].GetInt();
        }
        if (modelParam.HasMember("sb_box_size")) {
            sb_box_size = modelParam["sb_box_size"].GetInt();
        }
        if (modelParam.HasMember("class_size")) {
            class_size = modelParam["class_size"].GetUint();
        }
        if (modelParam.HasMember("stride")) {
            stride.clear();
            for (auto& str : modelParam["stride"].GetArray()) {
                stride.push_back(str.GetUint());
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
