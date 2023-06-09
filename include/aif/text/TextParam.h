/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_TEXT_PARAM_H
#define AIF_TEXT_PARAM_H

#include <aif/base/Types.h>
#include <aif/base/DetectorParam.h>
#include <opencv2/opencv.hpp>
#include <ostream>

namespace aif {

class TextParam : public DetectorParam
{
public:
    TextParam();
    virtual ~TextParam();
    TextParam(const TextParam& other);
    TextParam(TextParam&& other) noexcept;

    TextParam& operator=(const TextParam& other);
    TextParam& operator=(TextParam&& other) noexcept;

    bool operator==(const TextParam& other) const;
    bool operator!=(const TextParam& other) const;

    // debug
    friend std::ostream& operator<<(std::ostream& os, const TextParam& fp);
    void trace();

    t_aif_status fromJson(const std::string& param) override;

    float m_boxThreshold;
    bool m_useDetectionRegion;
    cv::Rect m_detectionRegion;
};

} // end of namespace aif

#endif // AIF_TEXT_PARAM_H
