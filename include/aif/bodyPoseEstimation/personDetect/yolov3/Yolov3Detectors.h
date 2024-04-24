/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_YOLOV3_DETECTORS_H
#define AIF_YOLOV3_DETECTORS_H

#include <aif/base/DetectorFactory.h>
#include <aif/base/DetectorFactoryRegistrations.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3/Yolov3Descriptor.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3/Yolov3Detector.h>

namespace aif {

class NpuYolov3V1Detector : public Yolov3Detector
{
    private:
        NpuYolov3V1Detector();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~NpuYolov3V1Detector() {}
};
DetectorFactoryRegistration<NpuYolov3V1Detector, Yolov3Descriptor>
    person_yolov3_v1_npu("person_yolov3_v1_npu");

class NpuYolov3V2Detector : public Yolov3Detector
{
    private:
        NpuYolov3V2Detector();
    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~NpuYolov3V2Detector() {}
};
DetectorFactoryRegistration<NpuYolov3V2Detector, Yolov3Descriptor>
    person_yolov3_v2_npu("person_yolov3_v2_npu");

} // end of namespace aif

#endif // AIF_YOLOV3_DETECTORS_H
