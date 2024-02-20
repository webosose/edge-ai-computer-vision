/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/pose2d/PostProcess.h>
#include <aif/bodyPoseEstimation/transforms.h>
#include <aif/log/Logger.h>

#include <limits>
#include <cmath>
#include <opencv2/opencv.hpp>

namespace aif {

template<typename T>
std::vector<T> flattenKeyPoints(std::vector<std::vector<T>> const &vec)
{
    std::vector<T> flattened;
    for (auto const &v: vec) {
        flattened.insert(flattened.end(), std::next(v.begin(), 1), v.end());
    }
    return flattened;
}

PostProcess::PostProcess(std::shared_ptr<Pose2dDetector>& detector)
    : m_modelInfo(detector->m_modelInfo)
    , m_cropRect(detector->m_cropRect)
    , m_paddedSize(detector->m_paddedSize)
    , m_cropScale(detector->m_cropScale)
    , m_cropBbox(detector->m_cropBbox)
    , m_useUDP(detector->m_useUDP)
    , m_leftBorder(detector->m_leftBorder)
    , m_topBorder(detector->m_topBorder)
    , m_numKeyPoints(detector->m_numKeyPoints)
    , m_heatMapWidth(DEFAULT_HEATMAP_WIDTH)
    , m_heatMapHeight(DEFAULT_HEATMAP_HEIGHT)
    , m_numInputs(1)
    , mTransMat(detector->mTransMat)
{
}

PostProcess::~PostProcess()
{
}

void PostProcess::getTransformMatrix(const BBox& bbox, float (&mul)[3][3]) const
{
    Logd(__func__ , " " , bbox.xmin , " " ,bbox.ymin , " " , bbox.width , " " , bbox.height);

    float centerX = bbox.xmin + (bbox.width * 0.5f);
    float centerY = bbox.ymin + (bbox.height * 0.5f);

    float cx = static_cast<float>(m_heatMapWidth - 1) / 2.0f;
    float cy = static_cast<float>(m_heatMapHeight - 1) / 2.0f;
    float df = cy - cx;

    float src[3][3] = {
            {cx, cy, 1.0f},
            {cx, df, 1.0f},
            {0.0f, df, 1.0f} };

    float icx = (1.0f - bbox.width) / 2.0f;
    float dx = centerX + icx;
    float dy = centerY + icx;
    float dst[3][3] = {
            {centerX, centerY, 0.0f},
            {centerX, dy, 0.0f},
            {dx, dy, 0.0f} };

    float det = 0.0f;
    for(auto i = 0; i < 3; i++) {
        auto ni = (i + 1) % 3;
        auto nni = (i + 2) % 3;
        float d = src[0][i] * ((src[1][ni] * src[2][nni]) - (src[1][nni] * src[2][ni]));
        det = det + d;
    }

    float inv[3][3];
    for(auto i = 0; i < 3; i++) {
        auto ni = (i + 1) % 3;
        auto nni = (i + 2) % 3;
        inv[i][0] = ((src[1][ni] * src[2][nni]) - (src[1][nni] * src[2][ni])) / det;
        inv[i][1] = ((src[2][ni] * src[0][nni]) - (src[2][nni] * src[0][ni])) / det;
        inv[i][2] = ((src[0][ni] * src[1][nni]) - (src[0][nni] * src[1][ni])) / det;
    }

    for(auto i = 0; i < 3; i++) {
        for(auto j = 0; j < 3; j++) {
            float sum = 0.0f;
            for(auto k = 0; k < 3; k++) {
                sum = sum + (inv[i][k] * dst[k][j]);
            }
            mul[i][j] = sum;
        }
    }
}


bool PostProcess::applyInverseTransform(std::vector<std::vector<float>>& keyPoints)
{
    std::vector<float> newJoints = flattenKeyPoints(keyPoints);

    if (mTransMat.empty()) {
        Loge("failed applyInverseTransform. mTransMat is empty.");
        return false;
    }

    cv::Mat transform;

    transform = getAffineTransform(
        cv::Point2f(m_cropBbox.c_x, m_cropBbox.c_y),
        cv::Point2f(m_cropScale.x, m_cropScale.y),
        0.0f, cv::Point2f(0, 0), m_heatMapWidth, m_heatMapHeight, false, true);

    cv::Mat inv_transform;
    cv::invertAffineTransform( transform, inv_transform );

    cv::Mat joints_mat(keyPoints.size(), 3, CV_32F, newJoints.data(), 0 );
    cv::Mat joints_mat_float;
    joints_mat.convertTo( joints_mat_float, inv_transform.type() );

    cv::Mat joints_mat_transposed;
    cv::transpose( joints_mat_float, joints_mat_transposed );

    cv::Mat result;
    result = inv_transform * joints_mat_transposed;
    result = result.t();

    std::vector<std::vector<float>> imageJoints;
    for (int i = 0; i<result.rows; i++) {
        cv::Mat mat = result.row(i);
        double *data = (double*)mat.data;
        std::vector<float> imageJoint = { keyPoints[i][0], static_cast<float>(data[0]), static_cast<float>(data[1])};
        imageJoints.push_back(imageJoint);
    }

    keyPoints = imageJoints;

    return true;
}

}
