/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NPU_YOLOV3_YC_V1_DETECTOR_H
#define AIF_NPU_YOLOV3_YC_V1_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3_yc_v1/Yolov3YCV1Descriptor.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3_yc_v1/Yolov3YCV1Detector.h>

namespace aif {

class NpuYolov3YCV1Detector : public Yolov3YCV1Detector {
  private:
    NpuYolov3YCV1Detector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~NpuYolov3YCV1Detector();
};

DetectorFactoryRegistration<NpuYolov3YCV1Detector, Yolov3YCV1Descriptor>
    person_yolov3_yc_v1_npu("person_yolov3_yc_v1_npu");

} // end of namespace aif

#endif // AIF_NPU_YOLOV3_YC_V1_DETECTOR_H
