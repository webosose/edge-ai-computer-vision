/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_FACE_MASK_EXTRACT_OPERATION_H
#define AIF_FACE_MASK_EXTRACT_OPERATION_H

#include <aif/pipe/BridgeOperation.h>
#include <opencv2/core/core.hpp>
#include <aif/pipe/NodeOperationFactoryRegistration.h>

namespace aif {

class FaceMaskExtractOperation : public BridgeOperation {
    private:
        FaceMaskExtractOperation(const std::string &id);

    public:
        bool runImpl(const std::shared_ptr<NodeInput> &input) override;
        virtual ~FaceMaskExtractOperation();
        template <typename T1, typename T2>
        friend class NodeOperationFactoryRegistration;
};
NodeOperationFactoryRegistration<FaceMaskExtractOperation, BridgeOperationConfig>
    face_mask_extract_operation("face_mask_extract");

} // end of namespace aif

#endif // AIF_FACE_MASK_EXTRACT_OPERATION_H
