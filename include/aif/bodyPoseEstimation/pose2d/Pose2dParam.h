/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_POSE2D_PARAM_H
#define AIF_POSE2D_PARAM_H

#include <aif/base/Types.h>
#include <aif/base/DetectorParam.h>

#include <ostream>

namespace aif {

class Pose2dParam : public DetectorParam
{
public:
    Pose2dParam();
    virtual ~Pose2dParam();
    Pose2dParam(const Pose2dParam& other);
    Pose2dParam(Pose2dParam&& other) noexcept;

    Pose2dParam& operator=(const Pose2dParam& other);
    Pose2dParam& operator=(Pose2dParam&& other) noexcept;

    bool operator==(const Pose2dParam& other) const;
    bool operator!=(const Pose2dParam& other) const;

    // debug
    friend std::ostream& operator<<(std::ostream& os, const Pose2dParam& fp);
    void trace();
};

} // end of namespace aif

#endif // AIF_POSE2D_PARAM_H
