/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_HANDLANDMARK_PARAM_H
#define AIF_HANDLANDMARK_PARAM_H

#include <aif/base/Types.h>
#include <aif/base/DetectorParam.h>

#include <memory>
#include <vector>
#include <ostream>

namespace aif {

class HandLandmarkParam : public DetectorParam
{
public:
    HandLandmarkParam();
    virtual ~HandLandmarkParam();
    HandLandmarkParam(const HandLandmarkParam& other);
    HandLandmarkParam(HandLandmarkParam&& other) noexcept;

    bool operator==(const HandLandmarkParam& other) const;
    bool operator!=(const HandLandmarkParam& other) const;

    // debug
    friend std::ostream& operator<<(std::ostream& os, const HandLandmarkParam& fp);
    void trace();
};

} // end of namespace aif

#endif // AIF_HANDLANDMARK_PARAM_H
