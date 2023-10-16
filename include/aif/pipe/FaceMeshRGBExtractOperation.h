/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_FACE_MESH_RGB_EXTRACT_OPERATION_H
#define AIF_FACE_MESH_RGB_EXTRACT_OPERATION_H

#include <aif/pipe/BridgeOperation.h>
#include <opencv2/core/core.hpp>
#include <aif/pipe/NodeOperationFactoryRegistration.h>

namespace aif {

class FaceMeshRGBExtractOperation : public BridgeOperation {
    private:
        FaceMeshRGBExtractOperation(const std::string &id);

    public:
        bool runImpl(const std::shared_ptr<NodeInput> &input) override;
        virtual ~FaceMeshRGBExtractOperation();
        template <typename T1, typename T2>
        friend class NodeOperationFactoryRegistration;
};
NodeOperationFactoryRegistration<FaceMeshRGBExtractOperation, BridgeOperationConfig>
    face_mesh_rgb_extract_operation("face_mesh_rgb_extract");

} // end of namespace aif

#endif // AIF_FACE_MESH_RGB_EXTRACT_OPERATION_H
