/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_POSE2D_DETECTOR_H
#define AIF_POSE2D_DETECTOR_H

#include <aif/base/Detector.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDescriptor.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dParam.h>

#include <aif/bodyPoseEstimation/common.h>
#include <vector>
#include <opencv2/opencv.hpp>

namespace aif {

class Pose2dDetector : public Detector
{
public:
    Pose2dDetector(const std::string& modelPath);
    virtual ~Pose2dDetector();

    enum {
        DEFAULT_HEATMAP_WIDTH = 48,
        DEFAULT_HEATMAP_HEIGHT = 64
    };
protected:
    std::shared_ptr<DetectorParam> createParam() override;
    void getPaddedImage(const cv::Mat& src, const cv::Size& modelSize, cv::Mat& dst);
    bool processHeatMap(
            const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor, float* heatMaps);

protected:
    cv::Rect m_cropRect;
    cv::Size m_paddedSize;
    int m_leftBorder;
    int m_numKeyPoints;
    int m_heatMapWidth;
    int m_heatMapHeight;
};

} // end of namespace aif

#endif  // AIF_POSE2D_DETECTOR_H
