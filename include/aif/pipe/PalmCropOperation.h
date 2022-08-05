/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_PALM_CROP_OPERATION_H
#define AIF_PALM_CROP_OPERATION_H

#include <aif/pipe/CropOperation.h>
#include <aif/pipe/NodeOperationFactoryRegistration.h>

namespace aif {

class PalmCropOperation : public CropOperation {
    private:
        PalmCropOperation(const std::string& id);

    protected:
        cv::Rect getCropRect(const std::shared_ptr<NodeInput>& input) const override;

    public:
        virtual ~PalmCropOperation();
        template<typename T1, typename T2>
            friend class NodeOperationFactoryRegistration;

};

NodeOperationFactoryRegistration<PalmCropOperation, BridgeOperationConfig>
    palm_crop_operation("palm_crop");

} // end of namespace aif

#endif // AIF_PALM_CROP_OPERATION_H
