/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_MOVENET_PARAM_H
#define AIF_MOVENET_PARAM_H

#include <aif/base/DetectorParam.h>
#include <aif/base/Types.h>

#include <memory>
#include <ostream>
#include <vector>

namespace aif {

class MovenetParam : public DetectorParam {
  public:
    MovenetParam();
    virtual ~MovenetParam();
    MovenetParam(const MovenetParam &other);
    MovenetParam(MovenetParam &&other) noexcept;

    bool operator==(const MovenetParam &other) const;
    bool operator!=(const MovenetParam &other) const;

    // debug
    friend std::ostream &operator<<(std::ostream &os, const MovenetParam &fp);
    void trace();
};

} // end of namespace aif

#endif // AIF_MOVENET_PARAM_H
