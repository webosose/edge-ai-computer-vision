#ifndef AIF_POSENET_DESCRIPTOR_H
#define AIF_POSENET_DESCRIPTOR_H

#include <aif/base/Descriptor.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

namespace aif {

class PosenetDescriptor: public Descriptor
{
public:
    PosenetDescriptor();
    virtual ~PosenetDescriptor();
    enum KeyPointType {
        NOSE = 0,
        LEFT_EYE = 1,
        RIGHT_EYE = 2,
        LEFT_EAR = 3,
        RIGHT_EAR = 4,
        LEFT_SHOULDER = 5,
        RIGHT_SHOULDER = 6,
        LEFT_ELBOW = 7,
        RIGHT_ELBOW = 8,
        LEFT_WRIST = 9,
        RIGHT_WRIST = 10,
        LEFT_HIP = 11,
        RIGHT_HIP = 12,
        LEFT_KNEE = 13,
        RIGHT_KNEE = 14,
        LEFT_ANKLE = 15,
        RIGHT_ANKLE = 16
    };
    void addKeyPoints(float score,
            const std::vector<cv::Point2f>& points,
            const std::vector<float>& keyPointsScore);
    cv::Rect2f getFaceRect(const std::vector<cv::Point2f>& keyPoints) const;
    cv::Rect2f getUpperBodyRect(const std::vector<cv::Point2f>& keyPoints) const;
    cv::Rect2f getBodyRect(const std::vector<cv::Point2f>& keyPoints) const;
    size_t getPoseCount() const { return m_keyPoints.size(); }
    std::vector<std::vector<cv::Rect2f>> makeBodyParts(std::vector<std::vector<cv::Rect2f>> prev);

protected:
    cv::Rect2f getRect(const std::vector<KeyPointType>& types,
        const std::vector<cv::Point2f>& keyPoints) const;
    float getScore(const std::vector<float>& scores, KeyPointType begin, KeyPointType end) const;
    bool isIOU(const cv::Rect2f& a, const cv::Rect2f& b, float threshold) const;
private:
    std::vector<float> m_scores;
    std::vector<std::vector<cv::Point2f>> m_keyPoints;
    std::vector<std::vector<float>> m_keyPointsScores;
    //std::vector<pair<KeyPointType, KeyPointType>> m_edges;
};

} // end of namespace aif

#endif // AIF_POSENET_DESCRIPTOR_H
