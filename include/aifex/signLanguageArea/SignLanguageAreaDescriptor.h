/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_SIGNLANGUAGEAREA_DESCRIPTOR_H
#define AIF_SIGNLANGUAGEAREA_DESCRIPTOR_H

#include <aif/base/Descriptor.h>
#include <aif/base/Types.h>

#include <vector>
#include <opencv2/opencv.hpp>

namespace aif {

class SignLanguageAreaDescriptor: public Descriptor
{
public:
    SignLanguageAreaDescriptor();
    virtual ~SignLanguageAreaDescriptor();

    bool addBoxes(const std::vector<float> &outputs);
};

} // end of namespace aif

#endif // AIF_SIGNLANGUAGEAREA_DESCRIPTOR_H
