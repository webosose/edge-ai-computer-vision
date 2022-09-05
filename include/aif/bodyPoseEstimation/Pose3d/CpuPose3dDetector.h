/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CPU_POSE3D_DETECTOR_H
#define AIF_CPU_POSE3D_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bodyPoseEstimation/Pose3d/Pose3dDescriptor.h>
#include <aif/bodyPoseEstimation/Pose3d/Pose3dDetector.h>

namespace aif {

class CpuPose3dDetector : public Pose3dDetector {
  private:
    CpuPose3dDetector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~CpuPose3dDetector();
};

DetectorFactoryRegistration<CpuPose3dDetector, Pose3dDescriptor>
    pose3d_videopose3d_cpu("pose3d_videopose3d_cpu");

} // end of namespace aif

#endif // AIF_CPU_POSE3D_DETECTOR_H
