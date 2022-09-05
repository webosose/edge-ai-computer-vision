/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_YOLOV4_DESCRIPTOR_H
#define AIF_YOLOV4_DESCRIPTOR_H

#include <aif/base/Descriptor.h>
#include <aif/base/Types.h>
#include <aif/bodyPoseEstimation/common.h>

namespace aif {

enum
{
    RESULT_YOLO_IDX_0 = 0,
    RESULT_YOLO_IDX_1,
    RESULT_YOLO_IDX_MAX
};

class Yolov4Descriptor: public Descriptor
{
public:
    Yolov4Descriptor();
    virtual ~Yolov4Descriptor();

    // TODO: implment add result
    void addPerson(float score, const BBox &bbox);
    void drawBbox(std::string imgPath);
    size_t size() const { return m_personCount; }
    void clear();

private:
    size_t m_personCount;
/*	std::vector<BBox> detectorBoxes;              // boxes in image coordinates
	std::vector<float> detectorScores;            // confidence for each box detection*/
};

} // end of namespace aif

#endif // AIF_YOLOV4_DESCRIPTOR_H
