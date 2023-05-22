/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_FACEMESH_DESCRIPTOR_H
#define AIF_FACEMESH_DESCRIPTOR_H

#include <aif/base/Descriptor.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

namespace aif {

const int FACEMESH_OVAL[] = {
    10, 338, 297, 332, 284, 301, 368, 264, 447, 366, 401, 435, 367, 364, 394, 395,
    369, 396, 175, 171, 140, 170, 169, 135, 138, 215, 177, 137, 227, 34, 139, 71, 54,
    103, 67, 109};
const int FACEMESH_LEFTEYE[] = {
    35, 113, 225, 224, 223, 222, 221, 189, 244, 233, 232, 231, 230, 229, 228, 31};
const int FACEMESH_RIGHTEYE[] = {
    265, 342, 445, 444, 443, 442, 441, 413, 464, 453, 452, 451, 450, 449, 448, 261};
const int FACEMESH_MOUTH[] = {
    212, 206, 129, 4, 358, 426, 432, 335, 18};

class FaceMeshDescriptor: public Descriptor
{
public:
    FaceMeshDescriptor();
    virtual ~FaceMeshDescriptor();

    void addLandmarks(const std::vector<cv::Vec3f> &points, double score);
    void clear();
};

} // end of namespace aif

#endif // AIF_FACEMESH_DESCRIPTOR_H
