/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_FACE_MESH_EXTRACT_OPERATION_H
#define AIF_FACE_MESH_EXTRACT_OPERATION_H

#include <aif/pipe/BridgeOperation.h>
#include <aif/pipe/FaceMeshExtractOperationConfig.h>
#include <aif/pipe/NodeOperationFactoryRegistration.h>

namespace aif {

class FaceMeshExtractOperation : public BridgeOperation {
    private:
        FaceMeshExtractOperation(const std::string &id);

    public:
        bool runImpl(const std::shared_ptr<NodeInput> &input) override;
        virtual ~FaceMeshExtractOperation();
        template <typename T1, typename T2>
        friend class NodeOperationFactoryRegistration;
};
NodeOperationFactoryRegistration<FaceMeshExtractOperation, FaceMeshExtractOperationConfig>
    face_mesh_extract_operation("face_mesh_extract");

} // end of namespace aif

#endif // AIF_FACE_MESH_EXTRACT_OPERATION_H
