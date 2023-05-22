/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_FACEMESH_PARAM_H
#define AIF_FACEMESH_PARAM_H

#include <aif/base/DetectorParam.h>
#include <aif/base/Types.h>

#include <memory>
#include <ostream>
#include <vector>

namespace aif {

class FaceMeshParam : public DetectorParam {
  public:
    FaceMeshParam();
    virtual ~FaceMeshParam();
    FaceMeshParam(const FaceMeshParam &other);
    FaceMeshParam(FaceMeshParam &&other) noexcept;

    bool operator==(const FaceMeshParam &other) const;
    bool operator!=(const FaceMeshParam &other) const;

    // debug
    friend std::ostream &operator<<(std::ostream &os, const FaceMeshParam &fp);
    void trace();
};

} // end of namespace aif

#endif // AIF_FACEMESH_PARAM_H
