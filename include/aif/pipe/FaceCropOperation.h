/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_FACE_CROP_OPERATION_H
#define AIF_FACE_CROP_OPERATION_H

#include <aif/pipe/CropOperation.h>
#include <aif/pipe/NodeOperationFactoryRegistration.h>

namespace aif {

class FaceCropOperation : public CropOperation
{
    private:
        FaceCropOperation(const std::string &id);
    protected:
        cv::Rect getCropRect(const std::shared_ptr<NodeInput> &input) const override;

    public:
        virtual ~FaceCropOperation();
        template <typename T1, typename T2>
        friend class NodeOperationFactoryRegistration;
};
NodeOperationFactoryRegistration<FaceCropOperation, BridgeOperationConfig>
    face_crop_operation("face_crop");
} // end of namespace aif

#endif // AIF_FACE_CROP_OPERATION_H
