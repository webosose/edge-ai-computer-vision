/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NPU_YOLOV4_DETECTOR_H
#define AIF_NPU_YOLOV4_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bodyPoseEstimation/personDetect/yolov4/Yolov4Descriptor.h>
#include <aif/bodyPoseEstimation/personDetect/yolov4/Yolov4Detector.h>

namespace aif {

class NpuYolov4Detector : public Yolov4Detector {
  private:
    NpuYolov4Detector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~NpuYolov4Detector();
};

DetectorFactoryRegistration<NpuYolov4Detector, Yolov4Descriptor>
    person_yolov4_npu("person_yolov4_npu");

} // end of namespace aif

#endif // AIF_NPU_YOLOV4_DETECTOR_H
