/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_BGSEGMENT_PARAM_H
#define AIF_BGSEGMENT_PARAM_H

#include <aif/base/DetectorParam.h>
#include <aif/base/Types.h>

#include <memory>
#include <ostream>
#include <vector>

namespace aif {

class BgSegmentParam : public DetectorParam {
  public:
    BgSegmentParam();
    virtual ~BgSegmentParam();
    BgSegmentParam(const BgSegmentParam &other);
    BgSegmentParam(BgSegmentParam &&other) noexcept;
    BgSegmentParam& operator=(const BgSegmentParam& other);
    BgSegmentParam& operator=(BgSegmentParam&& other) noexcept;

    bool operator==(const BgSegmentParam &other) const;
    bool operator!=(const BgSegmentParam &other) const;

    // debug
    friend std::ostream &operator<<(std::ostream &os, const BgSegmentParam &fp);
    void trace();

    t_aif_status fromJson(const std::string& param);
  public:
    int origImgRoiX;
    int origImgRoiY;
    int origImgRoiWidth;
    int origImgRoiHeight;
    bool outScaleUp;
    bool smoothing;
    float th_mad4;
};

} // end of namespace aif

#endif // AIF_BGSEGMENT_PARAM_H
