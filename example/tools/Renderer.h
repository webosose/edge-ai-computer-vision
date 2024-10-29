/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_RENDERER_H
#define AIF_RENDERER_H

#include <opencv2/opencv.hpp>

namespace aif {

class BBox;

class Renderer {
public:
    static cv::Mat drawPose2d(
            const cv::Mat &img,
            const std::vector<std::vector<float>>& keyPoints,
            bool drawAll = true);

    static cv::Mat drawRects(
            const cv::Mat &img,
            const std::vector<cv::Rect>& rects,
            cv::Scalar color = cv::Scalar(255, 0, 0),
            int thickness = 1);

    static cv::Mat drawBoxes(
            const cv::Mat &img,
            const std::vector<BBox>& boxes,
            cv::Scalar color = cv::Scalar(255, 0, 0),
            int thickness = 1);

    static cv::Mat drawRotatedRects(
            const cv::Mat &img,
            const std::vector<std::vector<cv::Point>>& rects,
            cv::Scalar color = cv::Scalar(0, 0, 255),
            int thickness = 1);

};

}

#endif // AIF_RENDERER_H
