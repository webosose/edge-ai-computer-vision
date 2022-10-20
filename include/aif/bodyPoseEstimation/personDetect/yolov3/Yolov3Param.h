/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_YOLOV3_PARAM_H
#define AIF_YOLOV3_PARAM_H

#include <aif/base/Types.h>
#include <aif/base/DetectorParam.h>
#include <aif/bodyPoseEstimation/common.h>
#include <aif/bodyPoseEstimation/BodyPoseEstimationParam.h>

#include <ostream>
#include <vector>

namespace aif {

class Yolov3Param : public BodyPoseEstimationParam
{
public:
    Yolov3Param();
    virtual ~Yolov3Param();
    Yolov3Param(const Yolov3Param& other);
    Yolov3Param(Yolov3Param&& other) noexcept;

    Yolov3Param& operator=(const Yolov3Param& other);
    Yolov3Param& operator=(Yolov3Param&& other) noexcept;

    bool operator==(const Yolov3Param& other) const;
    bool operator!=(const Yolov3Param& other) const;

    // debug
    friend std::ostream& operator<<(std::ostream& os, const Yolov3Param& fp);
    void trace();
    t_aif_status fromJson(const std::string& param) override;

public:
    std::string detectObject; // body or face
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
};

} // end of namespace aif

#endif // AIF_YOLOV3_PARAM_H
