#include <aif/bodyPoseEstimation/pose2d/Pose2dDetector.h>
#include <aif/bodyPoseEstimation/transforms.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <limits>
#include <cmath>

namespace aif {

Pose2dDetector::Pose2dDetector(const std::string& modelPath)
    : Detector(modelPath)
    , m_cropScale(0.0f)
    , m_boxCenter(0.0f, 0.0f)
    , m_useUDP(false)
    , m_leftBorder(0)
    , m_topBorder(0)
    , m_numKeyPoints(Pose2dDescriptor::KeyPointType::KEY_POINT_TYPES)
    , m_heatMapWidth(DEFAULT_HEATMAP_WIDTH)
    , m_heatMapHeight(DEFAULT_HEATMAP_HEIGHT)
{
}

Pose2dDetector::~Pose2dDetector()
{
}

std::shared_ptr<DetectorParam> Pose2dDetector::createParam()
{
    std::shared_ptr<DetectorParam> param = std::make_shared<Pose2dParam>();
    return param;
}

void Pose2dDetector::getAffinedImage(const cv::Mat& src, const cv::Size& modelSize, cv::Mat& dst)
{
    cv::Mat trans;
    const float scalingRate = 1.15f; // scaling for bbox-width

    trans = getAffineTransform( m_boxCenter,
                                cv::Point2f( m_cropScale * scalingRate, m_cropScale ),
                                0.0f, cv::Point2f( 0, 0 ), modelSize.width, modelSize.height,
                                false, true );

    trans.reshape( 1, trans.total() * trans.channels() );

    cv::Mat transformedImg = cv::Mat::zeros( modelSize.height, modelSize.width, src.type() );
    cv::warpAffine( src, transformedImg, trans, transformedImg.size() );

    dst = transformedImg;
    mTransMat = trans;
}

void Pose2dDetector::getPaddedImage(const cv::Mat& src, const cv::Size& modelSize, cv::Mat& dst)
{
    float srcW = src.size().width;
    float srcH = src.size().height;
    int modelW = modelSize.width;
    int modelH = modelSize.height;
    int dstW = 0;
    int dstH = 0;

    float scaleW = static_cast<float>(modelSize.width) / srcW;
    float scaleH = static_cast<float>(modelSize.height) / srcH;
    float scale = std::min(scaleW, scaleH);

    int width = srcW * scale;
    int height = srcH * scale;

    cv::Mat inputImg;
    cv::resize(src, inputImg, cv::Size(width, height), 0, 0, cv::INTER_LINEAR);

    m_leftBorder = 0;
    m_topBorder = 0;
    if (modelSize.width != width)
        m_leftBorder = (modelSize.width - width) / 2;
    if (modelSize.height != height)
        m_topBorder = (modelSize.height - height) / 2;

    int rightBorder = modelSize.width - width - m_leftBorder;
    int bottomBorder = modelSize.height - height - m_topBorder;

    cv::copyMakeBorder(inputImg, dst, m_topBorder, bottomBorder, m_leftBorder, rightBorder, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    m_paddedSize = cv::Size(
            srcW * (static_cast<float>(modelSize.width)/width),
            srcH * (static_cast<float>(modelSize.height)/height));
}

} // end of namespace aif
