/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_YOLOV4_PARAM_H
#define AIF_YOLOV4_PARAM_H

#include <aif/base/Types.h>
#include <aif/bodyPoseEstimation/common.h>
#include <aif/bodyPoseEstimation/BodyPoseEstimationParam.h>

#include <ostream>
#include <vector>

namespace aif {

class Yolov4Param : public BodyPoseEstimationParam
{
public:
    Yolov4Param();
    virtual ~Yolov4Param();
    Yolov4Param(const Yolov4Param& other);
    Yolov4Param(Yolov4Param&& other) noexcept;

    Yolov4Param& operator=(const Yolov4Param& other);
    Yolov4Param& operator=(Yolov4Param&& other) noexcept;

    bool operator==(const Yolov4Param& other) const;
    bool operator!=(const Yolov4Param& other) const;

    // debug
    friend std::ostream& operator<<(std::ostream& os, const Yolov4Param& fp);
    void trace();

    t_aif_status fromJson(const std::string& param) override;

public:
    int origImgRoiX;
    int origImgRoiY;
    int origImgRoiWidth;
    int origImgRoiHeight;
    int bboxBottomThresholdY;
    bool useFp16;
    double nms_threshold;
    double bbox_conf_threshold;
    double thresh_confidence;
    int numClasses;
    int numOutChannels;
    std::vector<int> strides;
    std::vector<int> numAnchors; // 3, 3
    std::vector<std::vector<std::pair<int, int>>> anchors;
    std::vector<std::vector<std::pair<float, float>>> anchorsNorm; // normalized anchor
};

} // end of namespace aif

#endif // AIF_YOLOV4_PARAM_H
