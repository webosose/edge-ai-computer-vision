/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_RPPG_PARAM_H
#define AIF_RPPG_PARAM_H

#include <aif/base/DetectorParam.h>
#include <aif/base/Types.h>

#include <memory>
#include <ostream>
#include <vector>

namespace aif {

class RppgParam : public DetectorParam {
  public:
    RppgParam();
    virtual ~RppgParam();
    RppgParam(const RppgParam &other);
    RppgParam(RppgParam &&other) noexcept;

    bool operator==(const RppgParam &other) const;
    bool operator!=(const RppgParam &other) const;

    // debug
    friend std::ostream &operator<<(std::ostream &os, const RppgParam &fp);
    void trace();
};

} // end of namespace aif

#endif // AIF_RPPG_PARAM_H
