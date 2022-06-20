/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_SELFIE_PARAM_H
#define AIF_SELFIE_PARAM_H

#include <aif/base/DetectorParam.h>
#include <aif/base/Types.h>

#include <memory>
#include <ostream>
#include <vector>

namespace aif {

class SelfieParam : public DetectorParam {
  public:
    SelfieParam();
    virtual ~SelfieParam();
    SelfieParam(const SelfieParam &other);
    SelfieParam(SelfieParam &&other) noexcept;

    bool operator==(const SelfieParam &other) const;
    bool operator!=(const SelfieParam &other) const;

    // debug
    friend std::ostream &operator<<(std::ostream &os, const SelfieParam &fp);
    void trace();
};

} // end of namespace aif

#endif // AIF_SELFIE_PARAM_H
