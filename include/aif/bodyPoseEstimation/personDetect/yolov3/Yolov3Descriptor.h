/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_YOLOV3_DESCRIPTOR_H
#define AIF_YOLOV3_DESCRIPTOR_H

#include <aif/base/Types.h>
#include <aif/bodyPoseEstimation/common.h>
#include <aif/bodyPoseEstimation/personDetect/PersonDetectDescriptor.h>

namespace aif {

class Yolov3Descriptor: public PersonDetectDescriptor
{
public:
    Yolov3Descriptor();
    virtual ~Yolov3Descriptor();

    // TODO: implment add result
    //void addPerson(float score, const BBox &bbox);
    virtual void addPerson(float score, const BBox &bbox);
    virtual void drawBbox(std::string imgPath);
    virtual void clear();

    void addFace(    /* only if face detect */
        float score,
        float region_x,
        float region_y,
        float region_w,
        float region_h);
    size_t sizeOfFaces() const { return m_faceCount; }

private:
    bool m_isBodyDetect;
    bool m_faceCount; /* only if face detect */
};

} // end of namespace aif

#endif // AIF_YOLOV3_DESCRIPTOR_H
