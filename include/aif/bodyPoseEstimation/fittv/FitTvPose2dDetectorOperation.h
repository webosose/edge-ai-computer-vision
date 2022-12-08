/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_FITTV_POSE2D_DETECTOR_OPERATION_H
#define AIF_FITTV_POSE2D_DETECTOR_OPERATION_H

#include <aif/pipe/DetectorOperation.h>
#include <aif/pipe/NodeOperationFactoryRegistration.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDetector.h>
#include <aif/base/Detector.h>

#include <string>

namespace aif {

class FitTvPose2dDetectorOperation : public DetectorOperation {
    private:
       FitTvPose2dDetectorOperation(const std::string& id);

    public:
       virtual ~FitTvPose2dDetectorOperation();

       template<typename T1, typename T2>
       friend class NodeOperationFactoryRegistration;

    protected:
       bool runImpl(const std::shared_ptr<NodeInput>& input) override;

};

NodeOperationFactoryRegistration<FitTvPose2dDetectorOperation, DetectorOperationConfig>
    fittv_pose2d_detector_operation("fittv_pose2d_detector");

} // end of namespace aif

#endif // AIF_FITTV_POSE2D_DETECTOR_OPERATION_H
