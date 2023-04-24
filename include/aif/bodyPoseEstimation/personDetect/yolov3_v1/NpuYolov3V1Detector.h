/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NPU_YOLOV3_V1_DETECTOR_H
#define AIF_NPU_YOLOV3_V1_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3_v1/Yolov3V1Descriptor.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3_v1/Yolov3V1Detector.h>

namespace aif {

class NpuYolov3V1Detector : public Yolov3V1Detector {
  private:
    NpuYolov3V1Detector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~NpuYolov3V1Detector();
};

DetectorFactoryRegistration<NpuYolov3V1Detector, Yolov3V1Descriptor>
    person_yolov3_v1_npu("person_yolov3_v1_npu");

} // end of namespace aif

#endif // AIF_NPU_YOLOV3_V1_DETECTOR_H
