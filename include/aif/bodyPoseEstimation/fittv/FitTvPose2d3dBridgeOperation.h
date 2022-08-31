/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_FITTV_POSE_2D_3D_BRIDGE_OPERATION_H
#define AIF_FITTV_POSE_2D_3D_BRIDGE_OPERATION_H

#include <aif/pipe/BridgeOperation.h>
#include <aif/pipe/NodeOperationFactoryRegistration.h>

namespace aif {

class FitTvPose2d3dBridgeOperation : public BridgeOperation {
    private:
        FitTvPose2d3dBridgeOperation(const std::string& id);

    public:
        virtual ~FitTvPose2d3dBridgeOperation();
        template<typename T1, typename T2>
            friend class NodeOperationFactoryRegistration;

        bool runImpl(const std::shared_ptr<NodeInput>& input) override;

};

NodeOperationFactoryRegistration<FitTvPose2d3dBridgeOperation, BridgeOperationConfig>
    fittv_pose2d3d("fittv_pose2d3d");

} // end of namespace aif

#endif // AIF_FITTV_POSE_2D_3D_BRIDGE_OPERATION_H
