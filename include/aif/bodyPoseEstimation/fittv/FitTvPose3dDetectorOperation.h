/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_FITTV_POSE3D_DETECTOR_OPERATION_H
#define AIF_FITTV_POSE3D_DETECTOR_OPERATION_H

#include <aif/pipe/DetectorOperation.h>
#include <aif/pipe/NodeOperationFactoryRegistration.h>
#include <aif/bodyPoseEstimation/pose3d/Pose3dDetector.h>
#include <aif/base/Detector.h>

#include <string>

namespace aif {

class FitTvPose3dDetectorOperation : public DetectorOperation {
    private:
       FitTvPose3dDetectorOperation(const std::string& id);

    public:
       virtual ~FitTvPose3dDetectorOperation();

       template<typename T1, typename T2>
       friend class NodeOperationFactoryRegistration;

    protected:
       bool runImpl(const std::shared_ptr<NodeInput>& input) override;

};

NodeOperationFactoryRegistration<FitTvPose3dDetectorOperation, DetectorOperationConfig>
    fittv_pose3d_detector_operation("fittv_pose3d_detector");

} // end of namespace aif

#endif // AIF_FITTV_POSE3D_DETECTOR_OPERATION_H
