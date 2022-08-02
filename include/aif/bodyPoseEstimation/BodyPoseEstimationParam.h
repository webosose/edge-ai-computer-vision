/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_BODYPOSEESTIMATION_PARAM_H
#define AIF_BODYPOSEESTIMATION_PARAM_H

#include <aif/base/Types.h>
#include <aif/base/DetectorParam.h>

#include <ostream>

namespace aif {

class BodyPoseEstimationParam : public DetectorParam
{
public:
    BodyPoseEstimationParam();
    virtual ~BodyPoseEstimationParam();
    BodyPoseEstimationParam(const BodyPoseEstimationParam& other);
    BodyPoseEstimationParam(BodyPoseEstimationParam&& other) noexcept;

    BodyPoseEstimationParam& operator=(const BodyPoseEstimationParam& other);
    BodyPoseEstimationParam& operator=(BodyPoseEstimationParam&& other) noexcept;

    bool operator==(const BodyPoseEstimationParam& other) const;
    bool operator!=(const BodyPoseEstimationParam& other) const;

    // debug
    friend std::ostream& operator<<(std::ostream& os, const BodyPoseEstimationParam& fp);
    void trace();

    t_aif_status fromJson(const std::string& param) override;

public:
    // common for BodyPoseEstimation
    int32_t orgImgWidth;
    int32_t orgImgHeight;
    int32_t numMaxPerson = 1;
    bool hasIntrinsics = 1;
    int32_t timeLogIntervalMs; /* TODO: NEEDED?? */

};

} // end of namespace aif

#endif // AIF_YOLOV4_PARAM_H
