#include <aif/bodyPoseEstimation/pose2d/Pose2dDetector.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <limits>

namespace aif {

Pose2dDetector::Pose2dDetector(const std::string& modelPath)
    : Detector(modelPath)
    , m_leftBorder(0)
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
        // scale heatmap to model size
        float x = (argMaxIndex % m_heatMapWidth) * (m_modelInfo.width / m_heatMapWidth);
        float y = (argMaxIndex / m_heatMapWidth) * (m_modelInfo.height / m_heatMapHeight);

        // scale model size to input img
        x = m_paddedSize.width  * ((x - m_leftBorder) / m_modelInfo.width);
        y = m_paddedSize.height * (y / m_modelInfo.height);

        // change x, y of input img to x, y of origin image
        x += m_cropRect.x;
        y += m_cropRect.y;

        keyPoints.push_back({ x, y, maxVal});
    }

    std::shared_ptr<Pose2dDescriptor> pose2dDescriptor =
        std::dynamic_pointer_cast<Pose2dDescriptor>(descriptor);

    pose2dDescriptor->addKeyPoints(keyPoints);
    return true;
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

    std::cout << "modelW : " << modelSize.width << std::endl;
    std::cout << "modelH : " << modelSize.height << std::endl;
    std::cout << "srcW : " << srcW << std::endl;
    std::cout << "srcH : " << srcH << std::endl;
    std::cout << "dstW : " << dstW << std::endl;
    std::cout << "dstH : " << dstH << std::endl;
    if (modelSize.width != width)
        m_leftBorder = (modelSize.width - width) / 2;
    int rightBorder = modelSize.width - width - m_leftBorder;

    cv::copyMakeBorder(inputImg, dst, 0, modelSize.height - height, m_leftBorder, rightBorder, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    m_paddedSize = cv::Size(
            srcW * (static_cast<float>(modelSize.width)/width),
            srcH * (static_cast<float>(modelSize.height)/height));
}

} // end of namespace aif
