/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_TEXT_DESCRIPTOR_H
#define AIF_TEXT_DESCRIPTOR_H

#include <aif/base/Descriptor.h>
#include <aif/base/Types.h>

#include <vector>
#include <opencv2/opencv.hpp>

namespace aif {

class TextDescriptor: public Descriptor
{
public:
    TextDescriptor();
    virtual ~TextDescriptor();

    bool addBboxes(const std::vector<cv::Rect>& bboxes);
    bool addBoxes(const std::vector<std::vector<cv::Point>>& boxes);

    size_t getTextRectCount() const { return m_textRectCount; }

private:
    size_t m_textRectCount;
};

} // end of namespace aif

#endif // AIF_TEXT_DESCRIPTOR_H
