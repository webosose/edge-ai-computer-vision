#include <aif/bodyPoseEstimation/pose2d/Pose2dDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <limits>
#include <cmath>

namespace aif {

Pose2dDetector::Pose2dDetector(const std::string& modelPath)
    : Detector(modelPath)
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

bool Pose2dDetector::processHeatMap(
        const cv::Mat& img, std::shared_ptr<Descriptor>& descriptor, float* heatMaps)
{
    std::vector<std::vector<float>> keyPoints;
    for (int k = 0; k < m_numKeyPoints; k++) {
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
        float y = (argMaxIndex / m_heatMapWidth);

        gaussianDark(heatMap, x, y);

        // scale heatmap to model size (x 4)
        x *= (m_modelInfo.width / m_heatMapWidth);
        y *= (m_modelInfo.height / m_heatMapHeight);

        // scale model size to input img
        x = m_paddedSize.width  * ((x - m_leftBorder) / m_modelInfo.width);
        y = m_paddedSize.height * ((y - m_topBorder) / m_modelInfo.height);

        // change x, y of input img to x, y of origin image
        x += m_cropRect.x;
        y += m_cropRect.y;

        keyPoints.push_back({ maxVal, x, y });
    }

    std::shared_ptr<Pose2dDescriptor> pose2dDescriptor =
        std::dynamic_pointer_cast<Pose2dDescriptor>(descriptor);

    pose2dDescriptor->addKeyPoints(keyPoints);
    return true;
}

void Pose2dDetector::gaussianBlur(float *heatMap, const int kernel) const
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

void Pose2dDetector::taylor(float *heatMap, float& coord_x, float& coord_y) const
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

        if (det != 0.f) {
            std::vector<std::pair<float, float>> hessianAdjointInv = { {dyy/det, -dxy/det}, {-dxy/det, dxx/det} };
            float offsetx = -( ( hessianAdjointInv[0].first * dx ) + ( hessianAdjointInv[0].second * dy ) );
            float offsety = -( ( hessianAdjointInv[1].first * dx ) + ( hessianAdjointInv[1].second * dy ) );

            coord_x = coord_x + offsetx;
            coord_y = coord_y + offsety;
        }
    }
}

void Pose2dDetector::gaussianDark(float *heatMap,
                                  float& coord_x, float& coord_y,
                                  const int BLUR_KERNEL) const
{
    gaussianBlur( heatMap, BLUR_KERNEL );
    for (int i = 0; i < m_heatMapWidth * m_heatMapHeight; i++) {
        heatMap[i] = std::log(std::max((double)heatMap[i], 1e-10)); // update heatmap by greater than 1e-10 and e logarithm of it
    }

    taylor( heatMap, coord_x, coord_y);
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
