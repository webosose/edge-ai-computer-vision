/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_SEMANTIC_DESCRIPTOR_H
#define AIF_SEMANTIC_DESCRIPTOR_H

#include <aif/base/Descriptor.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

namespace aif {

class SemanticDescriptor: public Descriptor
{
public:
    SemanticDescriptor();
    virtual ~SemanticDescriptor();

    void addMaskData(int width, int height, int64_t* mask);

private:
    std::vector<int> m_mask;
};

} // end of namespace aif

#endif // AIF_SEMANTIC_DESCRIPTOR_H
