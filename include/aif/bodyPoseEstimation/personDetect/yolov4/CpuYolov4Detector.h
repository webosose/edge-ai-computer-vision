/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CPU_YOLOV4_DETECTOR_H
#define AIF_CPU_YOLOV4_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bodyPoseEstimation/personDetect/yolov4/Yolov4Descriptor.h>
#include <aif/bodyPoseEstimation/personDetect/yolov4/Yolov4Detector.h>

namespace aif {

class CpuYolov4Detector : public Yolov4Detector {
  private:
    CpuYolov4Detector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~CpuYolov4Detector();
};

DetectorFactoryRegistration<CpuYolov4Detector, Yolov4Descriptor>
    person_yolov4_cpu("person_yolov4_cpu");

} // end of namespace aif

#endif // AIF_CPU_YOLOV4_DETECTOR_H
