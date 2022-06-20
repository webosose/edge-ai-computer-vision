/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_SELFIE_DESCRIPTOR_H
#define AIF_SELFIE_DESCRIPTOR_H

#include <aif/base/Descriptor.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

namespace aif {

class SelfieDescriptor: public Descriptor
{
public:
    SelfieDescriptor();
    virtual ~SelfieDescriptor();
    void addMaskData(int width, int height, float* mask);

private:
    std::vector<int> m_mask;

};

} // end of namespace aif

#endif // AIF_SELFIE_DESCRIPTOR_H
