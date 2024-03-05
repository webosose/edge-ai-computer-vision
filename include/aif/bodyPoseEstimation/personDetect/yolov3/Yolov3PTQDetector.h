/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_YOLOV3_PTQ_DETECTORS_H
#define AIF_YOLOV3_PTQ_DETECTORS_H

#include <aif/bodyPoseEstimation/personDetect/yolov3/Yolov3Detector.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3/Yolov3Descriptor.h>
#include <aif/base/DetectorFactoryRegistrations.h>

namespace aif {

class Yolov3PTQDetector : public Yolov3Detector
{
    private:
        Yolov3PTQDetector();
        virtual void setModelInOutInfo(const std::vector<int> &t_inputs,
                                   const std::vector<int> &t_outputs) override;


        virtual void Read_Result(std::vector<unsigned short> &obd_lb_box,
                                 std::vector<unsigned short> &obd_lb_conf,
                                 std::vector<unsigned short> &obd_mb_box,
                                 std::vector<unsigned short> &obd_mb_conf,
                                 std::vector<unsigned short> &obd_sb_box,
                                 std::vector<unsigned short> &obd_sb_conf) override;

        void RD_Result_box_conf(std::vector<unsigned short>& obd_box_conf, int8_t *addr);

    public:
        template <typename T1, typename T2>
            friend class DetectorFactoryRegistration;
        virtual ~Yolov3PTQDetector() {}
};
DetectorFactoryRegistration<Yolov3PTQDetector, Yolov3Descriptor>
    person_yolov3_ptq("person_yolov3_ptq");


} // end of namespace aif

#endif // AIF_YOLOV3_PTQ_DETECTORS_H


