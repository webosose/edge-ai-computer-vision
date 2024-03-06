/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_YUNET_FACE_PARAM_H
#define AIF_YUNET_FACE_PARAM_H

#include <aif/base/DetectorParam.h>

#include <memory>
#include <vector>
#include <string>
#include <ostream>

namespace aif {

class YuNetFaceParam : public DetectorParam
{
public:
    YuNetFaceParam();
    YuNetFaceParam(const YuNetFaceParam& other);
    YuNetFaceParam(YuNetFaceParam&& other) noexcept;

    YuNetFaceParam& operator=(const YuNetFaceParam& other);
    YuNetFaceParam& operator=(YuNetFaceParam&& other) noexcept;

    bool operator==(const YuNetFaceParam& other) const;
    bool operator!=(const YuNetFaceParam& other) const;

    // debug
    friend std::ostream& operator<<(std::ostream& os, const YuNetFaceParam& fp);
    void trace();


    t_aif_status fromJson(const std::string& param) override;

public:
    float scoreThreshold;
    float nmsThreshold;
    int topK;
};

} // end of namespace aif

#endif // AIF_YUNET_FACE_PARAM_H
