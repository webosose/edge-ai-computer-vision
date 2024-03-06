/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_BODYPIX_DESCRIPTOR_H
#define AIF_BODYPIX_DESCRIPTOR_H

#include <aif/base/Descriptor.h>
#include <aif/base/Types.h>

#include <vector>

namespace aif {

class BodypixDescriptor: public Descriptor
{
public:
    BodypixDescriptor();
    virtual ~BodypixDescriptor();

    void addMaskData(int width, int height, uint8_t* mask);

private:
    std::vector<int> m_mask;
};

} // end of namespace aif

#endif // AIF_BODYPIX_DESCRIPTOR_H
