/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NPU_POSE3D_DETECTOR_H
#define AIF_NPU_POSE3D_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bodyPoseEstimation/pose3d/Pose3dDescriptor.h>
#include <aif/bodyPoseEstimation/pose3d/Pose3dDetector.h>

namespace aif {

class NpuPose3dDetector : public Pose3dDetector {
  private:
    NpuPose3dDetector();

  public:
    template <typename T1, typename T2>
      friend class DetectorFactoryRegistration;
    virtual ~NpuPose3dDetector();
};

DetectorFactoryRegistration<NpuPose3dDetector, Pose3dDescriptor>
    pose3d_videopose3d_v1_npu("pose3d_videopose3d_v1_npu");

} // end of namespace aif

#endif // AIF_NPU_POSE3D_DETECTOR_H
