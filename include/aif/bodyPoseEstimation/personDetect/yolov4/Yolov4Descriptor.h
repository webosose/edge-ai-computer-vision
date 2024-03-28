/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_YOLOV4_DESCRIPTOR_H
#define AIF_YOLOV4_DESCRIPTOR_H

#include <aif/base/Types.h>
#include <aif/bodyPoseEstimation/common.h>
#include <aif/bodyPoseEstimation/personDetect/PersonDetectDescriptor.h>

namespace aif {

enum
{
    RESULT_YOLO_IDX_0 = 0,
    RESULT_YOLO_IDX_1,
    RESULT_YOLO_IDX_MAX
};

class Yolov4Descriptor: public PersonDetectDescriptor
{
public:
    Yolov4Descriptor();
    virtual ~Yolov4Descriptor();

    // TODO: implment add result
    virtual void addPerson(float score, const BBox &bbox, double confidenceThreshold = 0.0, const std::string& dbg_fname = "");
    virtual void drawBbox(std::string imgPath);
    virtual void clear();

};

} // end of namespace aif

#endif // AIF_YOLOV4_DESCRIPTOR_H
