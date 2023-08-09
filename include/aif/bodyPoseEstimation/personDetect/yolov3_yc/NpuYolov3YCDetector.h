/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NPU_YOLOV3_YC_DETECTOR_H
#define AIF_NPU_YOLOV3_YC_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3_yc/Yolov3YCDescriptor.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3_yc/Yolov3YCDetector.h>

namespace aif {

class NpuYolov3YCDetector : public Yolov3YCDetector {
  private:
    NpuYolov3YCDetector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~NpuYolov3YCDetector();
};

DetectorFactoryRegistration<NpuYolov3YCDetector, Yolov3YCDescriptor>
    person_yolov3_yc_npu("person_yolov3_yc_v1_npu");

} // end of namespace aif

#endif // AIF_NPU_YOLOV3_YC_DETECTOR_H
