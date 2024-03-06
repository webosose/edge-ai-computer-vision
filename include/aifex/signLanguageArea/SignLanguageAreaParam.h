/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_SIGNLANGUAGEAREA_PARAM_H
#define AIF_SIGNLANGUAGEAREA_PARAM_H

#include <aif/base/Types.h>
#include <aif/base/DetectorParam.h>
#include <opencv2/opencv.hpp>
#include <ostream>

namespace aif {

class SignLanguageAreaParam : public DetectorParam
{
public:
    SignLanguageAreaParam();
    virtual ~SignLanguageAreaParam();
    SignLanguageAreaParam(const SignLanguageAreaParam& other);
    SignLanguageAreaParam(SignLanguageAreaParam&& other) noexcept;

    bool operator==(const SignLanguageAreaParam& other) const;
    bool operator!=(const SignLanguageAreaParam& other) const;

    // debug
    friend std::ostream& operator<<(std::ostream& os, const SignLanguageAreaParam& fp);
    void trace();
};

} // end of namespace aif

#endif // AIF_SIGNLANGUAGEAREA_PARAM_H
