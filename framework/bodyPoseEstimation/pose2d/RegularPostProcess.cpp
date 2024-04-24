/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/pose2d/RegularPostProcess.h>
#include <aif/log/Logger.h>

#include <limits>
#include <cmath>

namespace aif {

RegularPostProcess::RegularPostProcess(std::shared_ptr<Pose2dDetector>& detector)
    : PostProcess(detector)
{
}

RegularPostProcess::~RegularPostProcess()
{
}

bool RegularPostProcess::execute(std::shared_ptr<Descriptor>& descriptor, float* data)
{
    Logd("Regular Post Process!!!");
    auto outputSize = m_heatMapWidth * m_heatMapHeight * m_numKeyPoints;
    float* buffer = new float[INT_TO_ULONG(outputSize)];
    std::memcpy(buffer, data, (outputSize * sizeof(float)));

    if(!processHeatMap(descriptor, buffer)) {
        delete [] buffer;
        return false;
    }

    delete [] buffer;
    return true;
}

bool RegularPostProcess::processHeatMap(std::shared_ptr<Descriptor>& descriptor, float* heatMaps)
{
    std::vector<std::vector<float>> keyPoints;

    for (int k = 0; k < m_numKeyPoints; k++) { //m_numKeyPoints = 41
        float* heatMap = heatMaps+ k * (m_heatMapWidth * m_heatMapHeight);
        float maxVal = std::numeric_limits<float>::min();
        int argMaxIndex = -1;
        for (int i = 0; i < m_heatMapHeight; i++) {
            for (int j = 0; j < m_heatMapWidth; j++) {
                if (maxVal < heatMap[i * m_heatMapWidth + j]) {
                    maxVal = heatMap[i * m_heatMapWidth + j];
                    argMaxIndex = i * m_heatMapWidth + j;
                }
            }
        }

        float x = (argMaxIndex % m_heatMapWidth);
        float y = ((float)argMaxIndex / (float)m_heatMapWidth);
    //--------------------------------------------
#if defined(GAUSSIANDARK)
        gaussianDark(heatMap, x, y);
#endif
        if (m_useUDP) {
            keyPoints.push_back({maxVal, x, y, 1.0});
        } else {
            int img_idx = 1 * m_numKeyPoints * 2;
            float mul[3][3];
            getTransformMatrix(m_cropBbox, mul);
            x = (x * mul[0][0]) + mul[2][0];
            y = (y * mul[1][1]) + mul[2][1];

            keyPoints.push_back({maxVal, x, y});
        }
    }

    if (m_useUDP) {
        if (!applyInverseTransform(keyPoints)) {
            return false;
        }
    }

    std::shared_ptr<Pose2dDescriptor> pose2dDescriptor =
        std::dynamic_pointer_cast<Pose2dDescriptor>(descriptor);
    if (pose2dDescriptor == nullptr) {
        Loge(__func__, "failed to convert Descriptor to Pose2dDescriptor");
        return false;
    }

    pose2dDescriptor->addKeyPoints(keyPoints);
    return true;
}

void RegularPostProcess::gaussianBlur(float *heatMap, const int kernel) const
{
    cv::Mat heatMap_mat(m_heatMapHeight, m_heatMapWidth, CV_32F, heatMap);
    double originMin, originMax;
    cv::minMaxLoc( heatMap_mat, &originMin, &originMax );
    cv::Mat heatMapBlur_mat;
    cv::GaussianBlur( heatMap_mat, heatMapBlur_mat, cv::Size( kernel, kernel ), 0., 0., cv::BORDER_CONSTANT);
    double newMin, newMax;
    cv::minMaxLoc( heatMapBlur_mat, &newMin, &newMax );

    int i = 0;
    for (auto it = heatMapBlur_mat.begin<float>(); it != heatMapBlur_mat.end<float>(); ++it) {
        heatMap[i++] = (*it) * (originMax / newMax);
    }
}

void RegularPostProcess::taylor(float *heatMap, float& coord_x, float& coord_y) const
{
    int px = static_cast<int>(coord_x);
    int py = static_cast<int>(coord_y);

    if ( ( 1 < px ) && ( px < ( m_heatMapWidth - 2 ) ) &&
                        ( 1 < py ) && ( py < ( m_heatMapHeight - 2 ) ) )
    {
        auto curLine = py * m_heatMapWidth;
        auto nextLine = ( py + 1 ) * m_heatMapWidth;
        auto prevLine = ( py - 1 ) * m_heatMapWidth;
        auto nnLine = ( py + 2 ) * m_heatMapWidth;
        auto ppLine = ( py - 2 ) * m_heatMapWidth;
        auto dx = 0.5f * ( heatMap[curLine + px + 1] - heatMap[curLine + px - 1] );
        auto dy = 0.5f * ( heatMap[nextLine + px] - heatMap[prevLine + px] );
        auto dxx = 0.25f *
                        ( heatMap[curLine + px + 2]
                                        - ( 2.f * heatMap[curLine + px] )
                                        + heatMap[curLine + px - 2] );
        auto dxy = 0.25f *
                        ( heatMap[nextLine + px + 1]
                                        - heatMap[prevLine + px + 1]
                                        - heatMap[nextLine + px - 1]
                                        + heatMap[prevLine + px - 1] );
        auto dyy = 0.25f *
                        ( heatMap[nnLine + px]
                        - ( 2.f * heatMap[curLine + px] )
                        + heatMap[ppLine + px] );

        float det = ( dxx * dyy ) - ( dxy * dxy );

        if (!(std::abs(det - 0.f) < aif::EPSILON)) {
            std::vector<std::pair<float, float>> hessianAdjointInv = { {dyy/det, -dxy/det}, {-dxy/det, dxx/det} };
            float offsetx = -( ( hessianAdjointInv[0].first * dx ) + ( hessianAdjointInv[0].second * dy ) );
            float offsety = -( ( hessianAdjointInv[1].first * dx ) + ( hessianAdjointInv[1].second * dy ) );

            coord_x = coord_x + offsetx;
            coord_y = coord_y + offsety;
        }
    }
}

void RegularPostProcess::gaussianDark(float *heatMap, float& coord_x, float& coord_y, const int BLUR_KERNEL) const
{
    gaussianBlur( heatMap, BLUR_KERNEL );
    for (int i = 0; i < m_heatMapWidth * m_heatMapHeight; i++) {
        heatMap[i] = std::log(std::max((double)heatMap[i], 1e-10)); // update heatmap by greater than 1e-10 and e logarithm of it
    }

    taylor( heatMap, coord_x, coord_y);
}

} // end of namespace aif
