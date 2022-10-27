/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_PERSONDETECT_DESCRIPTOR_H
#define AIF_PERSONDETECT_DESCRIPTOR_H

#include <aif/base/Descriptor.h>
#include <aif/base/Types.h>
#include <aif/bodyPoseEstimation/common.h>

namespace aif {

class PersonDetectDescriptor: public Descriptor
{
public:
    PersonDetectDescriptor() : m_personCount(0), m_IsBodyDetect(true) {}
    virtual ~PersonDetectDescriptor() {}

    // TODO: implment add result
    virtual void addPerson(float score, const BBox &bbox) = 0;
    virtual void drawBbox(std::string imgPath) = 0;
    size_t size() const { return m_personCount; }
    virtual void clear() = 0;

    int getNumBbox() const { return m_boxes.size(); }
    float getScore(int index) const { return m_scores[index]; }
    const BBox& getBbox(int index) const { return m_boxes[index]; }
    bool isBodyDetect() { return m_IsBodyDetect; }

protected:
    size_t m_personCount;
	std::vector<BBox> m_boxes;              // boxes in image coordinates
	std::vector<float> m_scores;            // confidence for each box detection
    bool m_IsBodyDetect;
};

} // end of namespace aif

#endif // AIF_PERSONDETECT_DESCRIPTOR_H
