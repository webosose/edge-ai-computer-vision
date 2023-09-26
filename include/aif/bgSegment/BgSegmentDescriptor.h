/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_BGSEGMENT_DESCRIPTOR_H
#define AIF_BGSEGMENT_DESCRIPTOR_H

#include <aif/base/Descriptor.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

namespace aif {

class BgSegmentDescriptor: public Descriptor
{
public:
    BgSegmentDescriptor();
    virtual ~BgSegmentDescriptor();

    void addMaskInfo(int x, int y, int width, int height);

private:
    std::vector<uint8_t> m_mask;
};

} // end of namespace aif

#endif // AIF_BGSEGMENT_DESCRIPTOR_H
