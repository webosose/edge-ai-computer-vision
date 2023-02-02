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
#include <memory>
#include <opencv2/opencv.hpp>

namespace aif {

class PostProcess;

class Pose2dDetector : public Detector
{
public:
    Pose2dDetector(const std::string& modelPath);
    virtual ~Pose2dDetector();

    void setCropData(float cropScale, cv::Point2f center) {
                             m_cropScale = cropScale;
                             m_boxCenter = center;
                             m_useUDP = true; }
    enum {
        DEFAULT_HEATMAP_WIDTH = 48,
        DEFAULT_HEATMAP_HEIGHT = 64
    };
protected:
    std::shared_ptr<DetectorParam> createParam() override;
    void getAffinedImage(const cv::Mat& src, const cv::Size& modelSize, cv::Mat& dst);
    void getPaddedImage(const cv::Mat& src, const cv::Size& modelSize, cv::Mat& dst);
    std::shared_ptr<PostProcess> m_postProcess;
protected:
    cv::Rect m_cropRect;
    float m_cropScale;
    cv::Point2f m_boxCenter;
    bool m_useUDP;
    cv::Size m_paddedSize;
    int m_leftBorder;
    int m_topBorder;
    int m_numKeyPoints;
    int m_heatMapWidth;
    int m_heatMapHeight;
    cv::Mat mTransMat;
public:
   friend class PostProcess;
};

} // end of namespace aif

#endif  // AIF_POSE2D_DETECTOR_H