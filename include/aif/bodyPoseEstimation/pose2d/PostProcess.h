/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_POST_PROCESS_H
#define AIF_POST_PROCESS_H

#include <aif/bodyPoseEstimation/pose2d/Pose2dDescriptor.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDetector.h>
#include <aif/base/Types.h>

#include <opencv2/opencv.hpp>

namespace aif {

class XtensorPostProcess;

class PostProcess
{
    public:
        PostProcess(std::shared_ptr<Pose2dDetector>& detector);
        virtual ~PostProcess();

        enum {
            DEFAULT_HEATMAP_WIDTH = 48,
            DEFAULT_HEATMAP_HEIGHT = 64
        };

        virtual bool execute(std::shared_ptr<Descriptor>& descriptor, float* data) = 0;

    protected:
        void getTransformMatrix(const BBox& bbox, float (&mul)[3][3]) const;
        bool applyInverseTransform(std::vector<std::vector<float>>& keyPoints);

    protected:
        t_aif_modelinfo m_modelInfo;
        Scale m_cropScale;
        BBox m_cropBbox;
        cv::Rect m_cropRect;
        cv::Size m_paddedSize;
        bool m_useUDP;
        int m_leftBorder;
        int m_topBorder;
        int m_numKeyPoints;
        int m_heatMapWidth;
        int m_heatMapHeight;
        int m_numInputs;
        cv::Mat mTransMat;
};

}

#endif  // AIF_POST_PROCESS_H