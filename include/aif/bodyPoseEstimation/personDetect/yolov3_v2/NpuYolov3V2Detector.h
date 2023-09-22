/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NPU_YOLOV3_V2_DETECTOR_H
#define AIF_NPU_YOLOV3_V2_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3_v2/Yolov3V2Descriptor.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3_v2/Yolov3V2Detector.h>

namespace aif {

class NpuYolov3V2Detector : public Yolov3V2Detector {
  private:
    NpuYolov3V2Detector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~NpuYolov3V2Detector();
};

DetectorFactoryRegistration<NpuYolov3V2Detector, Yolov3V2Descriptor>
    person_yolov3_v2_npu("person_yolov3_v2_npu");

} // end of namespace aif

#endif // AIF_NPU_YOLOV3_V2_DETECTOR_H
