/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_RENDERER_H
#define AIF_RENDERER_H

#include <opencv2/opencv.hpp>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDescriptor.h>

namespace aif {

class Renderer {
public:
    static cv::Mat drawPose2d(
            const cv::Mat &img,
            const std::vector<std::vector<float>>& keyPoints);
};

}

#endif // AIF_RENDERER_H
