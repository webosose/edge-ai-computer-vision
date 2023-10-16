/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_RPPG_DESCRIPTOR_H
#define AIF_RPPG_DESCRIPTOR_H

#include <aif/base/Descriptor.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

namespace aif {

class RppgDescriptor: public Descriptor
{
public:
    RppgDescriptor();
    virtual ~RppgDescriptor();

    void addRppgOutput(float bpm, std::string signalCondition);
    void clear();
};

} // end of namespace aif

#endif // AIF_RPPG_DESCRIPTOR_H
