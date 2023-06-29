/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_YOLOV3_YC_V1_DESCRIPTOR_H
#define AIF_YOLOV3_YC_V1_DESCRIPTOR_H

#include <aif/base/Types.h>
#include <aif/bodyPoseEstimation/common.h>
#include <aif/bodyPoseEstimation/personDetect/PersonDetectDescriptor.h>
#include <aif/face/FaceDescriptor.h>

namespace aif {

class Yolov3YCV1Descriptor: public PersonDetectDescriptor, public FaceDescriptor
{
public:
    Yolov3YCV1Descriptor();
    virtual ~Yolov3YCV1Descriptor();

    // TODO: implment add result
    virtual void addPerson(float score, const BBox &bbox);
    virtual void drawBbox(std::string imgPath);
    virtual void clear();

    size_t sizeOfFaces() const { return FaceDescriptor::size(); }
    size_t sizeOfPersons() const { return PersonDetectDescriptor::size(); }
private:
};

} // end of namespace aif

#endif // AIF_YOLOV3_YC_V1_DESCRIPTOR_H
