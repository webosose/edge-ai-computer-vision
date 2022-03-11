#include <aif/pose/PosenetDescriptor.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>
#include <limits>
#include <cmath>

namespace {
} // anonymous namespace

namespace rj = rapidjson;

namespace aif {

PosenetDescriptor::PosenetDescriptor()
    : Descriptor()
{
}

PosenetDescriptor::~PosenetDescriptor()
{
}

void PosenetDescriptor::addKeyPoints(float score,
            const std::vector<cv::Point2f>& points,
            const std::vector<float>& keyPointsScore)
{
    m_scores.push_back(score);
    m_keyPoints.push_back(points);
    m_keyPointsScores.push_back(keyPointsScore);
}

cv::Rect2f PosenetDescriptor::getRect(const std::vector<KeyPointType>& types,
        const std::vector<cv::Point2f>& keyPoints) const
{
    float left, right, top, bottom;
    left = top = std::numeric_limits<int>::max();
    right = bottom = 0;
    for (auto type : types) {
        left = std::min(left, keyPoints[type].x);
        right = std::max(right, keyPoints[type].x);
        top = std::min(top, keyPoints[type].y);
        bottom = std::max(bottom, keyPoints[type].y);
    }
    float scaleW = 1.2;
    float scaleH = 1.2;
    float width = (right - left) * scaleW;
    float height = (bottom - top) * scaleH;
    float x = (left + right) / 2.0 - (width / 2.0) ;
    float y = (top + bottom) / 2.0 - (height/ 2.0) ;
    return cv::Rect2f(x, y, width, height);
}

cv::Rect2f PosenetDescriptor::getFaceRect(const std::vector<cv::Point2f>& keyPoints) const {
    float scaleH = 1.2;
    std::vector<KeyPointType> types;
    for (int type = NOSE; type <= RIGHT_EAR; type++) {
        types.emplace_back(static_cast<KeyPointType>(type));
    }
    cv::Rect2f rect = getRect(types, keyPoints);
    float width = rect.width;
    float height = width * scaleH;
    float x = rect.x;
    float y = keyPoints[KeyPointType::NOSE].y - (height / 2.0);

    cv::Rect2f faceRect(x, y, width, height);
    return faceRect;
}

cv::Rect2f PosenetDescriptor::getUpperBodyRect(const std::vector<cv::Point2f>& keyPoints) const {
    std::vector<KeyPointType> types;
    for (int type = NOSE; type <= RIGHT_HIP; type++) {
        types.emplace_back(static_cast<KeyPointType>(type));
    }
    return getRect(types, keyPoints);
}

cv::Rect2f PosenetDescriptor::getBodyRect(const std::vector<cv::Point2f>& keyPoints) const {
    std::vector<KeyPointType> types;
    for (int type = NOSE; type <= RIGHT_ANKLE; type++) {
        types.emplace_back(static_cast<KeyPointType>(type));
    }
    return getRect(types, keyPoints);
}

float PosenetDescriptor::getScore(const std::vector<float>& scores, KeyPointType begin, KeyPointType end) const {
    float score = 0;
    for (int i = begin; i <= end; i++) {
        score += scores[i];
    }
    score = score / (end - begin + 1);
    return score;
}

std::vector<std::vector<cv::Rect2f>>
PosenetDescriptor::makeBodyParts(std::vector<std::vector<cv::Rect2f>> prev, float iouThreshold)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("poses")) {
        rj::Value poses(rj::kArrayType);
        m_root.AddMember("poses", poses, allocator);
    }

    std::vector<std::vector<cv::Rect2f>> current;
    for (int i = 0; i < m_keyPoints.size(); i++) {
        auto& keyPoints = m_keyPoints[i];
        auto& scores = m_keyPointsScores[i];

        rj::Value person(rj::kObjectType);
        cv::Rect2f faceRect = getFaceRect(keyPoints);

        if (i < prev.size() && !isIOU(faceRect, prev[i][0], iouThreshold)) {
            faceRect = prev[i][0];
        }
        rj::Value face(rj::kArrayType);
        face.PushBack(faceRect.x, allocator)
            .PushBack(faceRect.y, allocator)
            .PushBack(faceRect.width, allocator)
            .PushBack(faceRect.height, allocator);
        person.AddMember("face", face, allocator);
        person.AddMember("face_score", getScore(scores, NOSE, RIGHT_EAR), allocator);

        cv::Rect bodyRect = getBodyRect(keyPoints);
        if (i < prev.size() && !isIOU(bodyRect, prev[i][1], iouThreshold)) {
            bodyRect = prev[i][1];
        }
        rj::Value body(rj::kArrayType);
        body.PushBack(bodyRect.x, allocator)
            .PushBack(bodyRect.y, allocator)
            .PushBack(bodyRect.width, allocator)
            .PushBack(bodyRect.height, allocator);
        person.AddMember("body", body, allocator);
        person.AddMember("body_score", getScore(scores, NOSE, RIGHT_ANKLE), allocator);

        cv::Rect upperBodyRect = getUpperBodyRect(keyPoints);
        if (i < prev.size() && !isIOU(upperBodyRect, prev[i][2], iouThreshold)) {
            upperBodyRect = prev[i][2];
        }
        rj::Value upperBody(rj::kArrayType);
        upperBody.PushBack(upperBodyRect.x, allocator)
            .PushBack(upperBodyRect.y, allocator)
            .PushBack(upperBodyRect.width, allocator)
            .PushBack(upperBodyRect.height, allocator);
        person.AddMember("upperBody", upperBody, allocator);
        person.AddMember("upperBody_score", getScore(scores, NOSE, RIGHT_HIP), allocator);

        person.AddMember("score", m_scores[i], allocator);
        rj::Value points(rj::kArrayType);
        for (int j = 0; j < NUM_KEYPOINT_TYPES; j++) {
            rj::Value point(rj::kArrayType);
            point.PushBack(keyPoints[j].x, allocator);
            point.PushBack(keyPoints[j].y, allocator);
            points.PushBack(point, allocator);
        }
        person.AddMember("keyPoints", points, allocator);
        m_root["poses"].PushBack(person, allocator);

        current.push_back({faceRect, bodyRect, upperBodyRect});
    }

    return current;
}

}

