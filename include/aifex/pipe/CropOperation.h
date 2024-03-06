/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CROP_OPERATION_H
#define AIF_CROP_OPERATION_H

#include <aif/pipe/BridgeOperation.h>
#include <opencv2/core/core.hpp>

namespace aif {

class CropOperation : public BridgeOperation {
    protected:
       CropOperation(const std::string& id);
       virtual ~CropOperation();
       virtual cv::Rect getCropRect(const std::shared_ptr<NodeInput>& input) const = 0;

    public:
       bool runImpl(const std::shared_ptr<NodeInput>& input) override;

};

} // end of namespace aif

#endif // AIF_CROP_OPERATION_H
