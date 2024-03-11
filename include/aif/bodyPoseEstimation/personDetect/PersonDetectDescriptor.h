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

class PersonDetectDescriptor: virtual public Descriptor
{
public:
    PersonDetectDescriptor() : m_numNonDetected(0), m_personCount(0), m_IsBodyDetect(true) {}
    virtual ~PersonDetectDescriptor() {}

    // TODO: implment add result
    virtual void addPerson(float score, const BBox &bbox, const std::string& dbg_fname = "") = 0;
    virtual void drawBbox(std::string imgPath) = 0;
    virtual void clear() = 0;

    size_t size() const { return m_personCount; }

    void addRoiRect(const cv::Rect &roi, bool valid) { m_roiRect = roi; m_roiValid = valid; }
    void addNumNonDetected(int count) { m_numNonDetected = count; }
    int getNumBbox() const { return m_boxes.size(); }
    int getNumNonDetected() const { return m_numNonDetected; }
    float getScore(int index) const { return m_scores[index]; }
    const std::string& getDbgFileName() { return m_dbg_fname; }
    const BBox& getBbox(int index) const { return m_boxes[index]; }
    const cv::Rect& getRoiRect() const { return m_roiRect; }
    bool isBodyDetect() { return m_IsBodyDetect; }
    bool isRoiValid() { return m_roiValid; }

protected:
    int m_numNonDetected;
    size_t m_personCount;
    std::vector<BBox> m_boxes;              // boxes in image coordinates
    std::vector<float> m_scores;            // confidence for each box detection
    std::string m_dbg_fname;
    cv::Rect m_roiRect;
    bool m_roiValid;
    bool m_IsBodyDetect;
};

} // end of namespace aif

#endif // AIF_PERSONDETECT_DESCRIPTOR_H
