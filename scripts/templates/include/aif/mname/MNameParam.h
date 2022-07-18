/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_MNAME_PARAM_H
#define AIF_MNAME_PARAM_H

#include <aif/base/Types.h>
#include <aif/base/DetectorParam.h>

#include <ostream>

namespace aif {

class MNameParam : public DetectorParam
{
public:
    MNameParam();
    virtual ~MNameParam();
    MNameParam(const MNameParam& other);
    MNameParam(MNameParam&& other) noexcept;

    MNameParam& operator=(const MNameParam& other);
    MNameParam& operator=(MNameParam&& other) noexcept;

    bool operator==(const MNameParam& other) const;
    bool operator!=(const MNameParam& other) const;

    // debug
    friend std::ostream& operator<<(std::ostream& os, const MNameParam& fp);
    void trace();
};

} // end of namespace aif

#endif // AIF_MNAME_PARAM_H
