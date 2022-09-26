/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NPU_YOLOV3_DETECTOR_H
#define AIF_NPU_YOLOV3_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3/Yolov3Descriptor.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3/Yolov3Detector.h>

namespace aif {

class NpuYolov3Detector : public Yolov3Detector {
  private:
    NpuYolov3Detector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~NpuYolov3Detector();
};

DetectorFactoryRegistration<NpuYolov3Detector, Yolov3Descriptor>
    person_yolov3_npu("person_yolov3_npu");

} // end of namespace aif

#endif // AIF_NPU_YOLOV3_DETECTOR_H
