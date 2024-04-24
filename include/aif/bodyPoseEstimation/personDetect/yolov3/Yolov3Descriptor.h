/*
 * Copyright (c) 2024 LG Electronics Inc.
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
        virtual void addPerson(float score, const BBox &bbox, double confidenceThreshold = 0.0, const std::string& dbg_fname = "");
        virtual void drawBbox(std::string imgPath);
        void addFace(float score, float region_x, float region_y, float region_w, float region_h,
                     float lefteye_x, float lefteye_y, float righteye_x, float righteye_y, float nosetip_x,
                     float nosetip_y, float mouth_x, float mouth_y,float leftear_x, float leftear_y,
                     float rightear_x, float rightear_y, double confidenceThreshold = 0.0);
        virtual void clear();

        size_t sizeOfFaces() const { return m_faceCount; }
        size_t sizeOfPersons() const { return PersonDetectDescriptor::size(); }

    private:
        size_t m_faceCount;
};

} // end of namespace aif

#endif // AIF_YOLOV3_DESCRIPTOR_H
