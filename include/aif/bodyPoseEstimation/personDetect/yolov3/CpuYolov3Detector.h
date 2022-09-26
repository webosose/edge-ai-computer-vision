/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CPU_YOLOV3_DETECTOR_H
#define AIF_CPU_YOLOV3_DETECTOR_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3/Yolov3Descriptor.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3/Yolov3Detector.h>

namespace aif {

class CpuYolov3Detector : public Yolov3Detector {
  private:
    CpuYolov3Detector();

  public:
    template <typename T1, typename T2>
    friend class DetectorFactoryRegistration;
    virtual ~CpuYolov3Detector();
};

DetectorFactoryRegistration<CpuYolov3Detector, Yolov3Descriptor>
    person_yolov3_cpu("person_yolov3_cpu");

} // end of namespace aif

#endif // AIF_CPU_YOLOV3_DETECTOR_H
