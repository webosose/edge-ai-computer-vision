/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_YOLOV3_V2_PARAM_H
#define AIF_YOLOV3_V2_PARAM_H

#include <aif/base/Types.h>
#include <aif/base/DetectorParam.h>
#include <aif/bodyPoseEstimation/common.h>
#include <aif/bodyPoseEstimation/BodyPoseEstimationParam.h>

#include <ostream>
#include <vector>

namespace aif {

class Yolov3V2Param : public BodyPoseEstimationParam
{
public:
    Yolov3V2Param();
    virtual ~Yolov3V2Param();
    Yolov3V2Param(const Yolov3V2Param& other);
    Yolov3V2Param(Yolov3V2Param&& other) noexcept;

    Yolov3V2Param& operator=(const Yolov3V2Param& other);
    Yolov3V2Param& operator=(Yolov3V2Param&& other) noexcept;

    bool operator==(const Yolov3V2Param& other) const;
    bool operator!=(const Yolov3V2Param& other) const;

    // debug
    friend std::ostream& operator<<(std::ostream& os, const Yolov3V2Param& fp);
    void trace();
    t_aif_status fromJson(const std::string& param) override;

public:
    std::string detectObject; // body or face
    //std::vector<float> gt_bbox; // x, y, w, h
    std::vector<std::pair<std::string, std::vector<float>>> gt_bboxes; // [[str, x, y, w, h], []...]
    int origImgRoiX;
    int origImgRoiY;
    int origImgRoiWidth;
    int origImgRoiHeight;
    int lbbox_h;
    int lbbox_w;
    int mbbox_h;
    int mbbox_w;
    int box_size;
    int class_size;
    std::vector<int> stride; // lbbox, mbbox
    std::vector<int> thresh_score; // LLL, L, M, H
    int thresh_iou_sc_nms;
    int thresh_iou_sc_sur;
    int thresh_iou_sc_avg;
    float thresh_iou_update;
};

} // end of namespace aif

#endif // AIF_YOLOV3_V2_PARAM_H
