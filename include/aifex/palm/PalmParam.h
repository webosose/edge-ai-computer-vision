/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_PALM_PARAM_H
#define AIF_PALM_PARAM_H

#include <aif/base/Types.h>
#include <aif/base/DetectorParam.h>

#include <memory>
#include <vector>
#include <ostream>

namespace aif {

class PalmParam : public DetectorParam
{
public:
    PalmParam();
    virtual ~PalmParam();
    PalmParam(const PalmParam& other);
    PalmParam(PalmParam&& other) noexcept;

    PalmParam& operator=(const PalmParam& other);
    PalmParam& operator=(PalmParam&& other) noexcept;

    bool operator==(const PalmParam& other) const;
    bool operator!=(const PalmParam& other) const;

    // debug
    friend std::ostream& operator<<(std::ostream& os, const PalmParam& fp);
    void trace();
    
    t_aif_status fromJson(const std::string& param) override;

public:
    // for anchor
    std::vector<int> strides;
    std::vector<float> optAspectRatios;
    float interpolatedScaleAspectRatio;
    float anchorOffsetX;
    float anchorOffsetY;
    float minScale;
    float maxScale;
    bool  reduceBoxesInLowestLayer = false;
    // for detect
    float scoreThreshold;
    float iouThreshold;
    int   maxOutputSize;
    float updateThreshold;
};

} // end of namespace aif

#endif // AIF_PALM_PARAM_H
