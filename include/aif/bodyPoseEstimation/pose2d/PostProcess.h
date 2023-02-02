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
    PostProcess(std::shared_ptr<Pose2dDetector>& detector)
        : m_modelInfo(detector->m_modelInfo)
        , m_cropRect(detector->m_cropRect)
        , m_paddedSize(detector->m_paddedSize)
        , m_useUDP(detector->m_useUDP)
        , m_leftBorder(detector->m_leftBorder)
        , m_topBorder(detector->m_topBorder)
        , m_numKeyPoints(detector->m_numKeyPoints)
        , m_heatMapWidth(DEFAULT_HEATMAP_WIDTH)
        , m_heatMapHeight(DEFAULT_HEATMAP_HEIGHT)
        , m_numInputs(1)
        , mTransMat(detector->mTransMat)
    {};
    virtual ~PostProcess() {};

    enum {
        DEFAULT_HEATMAP_WIDTH = 48,
        DEFAULT_HEATMAP_HEIGHT = 64
    };

    virtual bool execute(std::shared_ptr<Descriptor>& descriptor, float* data) = 0;
protected:
    template<typename T>
    std::vector<T> flattenKeyPoints(std::vector<std::vector<T>> const &vec)
    {
        std::vector<T> flattened;
        for (auto const &v: vec) {
            flattened.insert(flattened.end(), v.begin() + 1, v.end());
        }
        return flattened;
    }
protected:
    t_aif_modelinfo m_modelInfo;
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