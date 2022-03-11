#include <aif/poseLandmark/PoseLandmarkDescriptor.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>
#include <limits>

namespace {
} // anonymous namespace

namespace rj = rapidjson;

namespace aif {

PoseLandmarkDescriptor::PoseLandmarkDescriptor()
    : Descriptor()
    , m_landmarks(NUM_LANDMARK_TYPES)
{
}

PoseLandmarkDescriptor::~PoseLandmarkDescriptor()
{
}

void PoseLandmarkDescriptor::addMaskData(int width, int height, float* mask)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("segments")) {
        rj::Value segments(rj::kArrayType);
        m_root.AddMember("segments", segments, allocator);
    }

    rj::Value segment(rj::kObjectType);
    rj::Value maskData(rj::kArrayType);
    int j = 0;
    for (int i = 0; i < width * height; i++) {
        if (sigmoid<float>(mask[j++]) > 0.1f){
            maskData.PushBack(1, allocator);
        }
        else {
            maskData.PushBack(0, allocator);
        }
    }
    segment.AddMember("width", width, allocator);
    segment.AddMember("height", height, allocator);
    segment.AddMember("mask", maskData, allocator);
    m_root["segments"].PushBack(segment, allocator);
}

void PoseLandmarkDescriptor::addLandmarks(float* landmarks)
{
    for (int i = 0; i < LandmarkType::NUM_LANDMARK_TYPES; i++) {
        for (int j = 0; j < NUM_LANDMARK_ITEMS; j++) {
            int index = i * NUM_LANDMARK_ITEMS + j;
            float data = landmarks[index];
            if (j <= LandmarkOutput::COOD_Z) {
                data = data / 256;              // normalize
            } else {
                data = sigmoid<float>(data);    // sigmoid
            }
            m_landmarks[i].emplace_back(data);
        }
    }
}

std::vector<std::vector<cv::Rect2f>>
PoseLandmarkDescriptor::makeBodyParts(std::vector<std::vector<cv::Rect2f>> prev, float iouThreshold)
{
    if (!m_landmarks[0].size()) {
        return {{}};
    }

    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("poses")) {
        rj::Value poses(rj::kArrayType);
        m_root.AddMember("poses", poses, allocator);
    }
    rj::Value person(rj::kObjectType);
    rj::Value points(rj::kArrayType);

    for (int i = 0; i < m_landmarks.size(); i++) {
        rj::Value point(rj::kArrayType);
        for (int j = 0; j < m_landmarks[i].size(); j++) {
            point.PushBack(m_landmarks[i][j], allocator);
        }
        points.PushBack(point, allocator);
    }
    person.AddMember("landmarks", points, allocator);


    int i = 0;
    cv::Rect2f faceRect = getRect(NOSE, MOUTH_RIGHT);
    if (i < prev.size() && !isIOU(faceRect, prev[i][0], iouThreshold)) {
        faceRect = prev[i][0];
    }
    rj::Value face(rj::kArrayType);
    face.PushBack(faceRect.x, allocator)
        .PushBack(faceRect.y, allocator)
        .PushBack(faceRect.width, allocator)
        .PushBack(faceRect.height, allocator);
    person.AddMember("face", face, allocator);
    person.AddMember("face_score", getScore(NOSE, MOUTH_RIGHT), allocator);

    cv::Rect2f bodyRect = getRect(NOSE, RIGHT_FOOT_INDEX);
    if (i < prev.size() && !isIOU(bodyRect, prev[i][1], iouThreshold)) {
        bodyRect = prev[i][1];
    }
    rj::Value body(rj::kArrayType);
    body.PushBack(bodyRect.x, allocator)
        .PushBack(bodyRect.y, allocator)
        .PushBack(bodyRect.width, allocator)
        .PushBack(bodyRect.height, allocator);
    person.AddMember("body", body, allocator);
    person.AddMember("body_score", getScore(NOSE, RIGHT_FOOT_INDEX), allocator);

    cv::Rect2f upperBodyRect = getRect(NOSE, RIGHT_HIP);
    if (i < prev.size() && !isIOU(upperBodyRect, prev[i][2], iouThreshold)) {
        upperBodyRect = prev[i][2];
    }
    rj::Value upperBody(rj::kArrayType);
    upperBody.PushBack(upperBodyRect.x, allocator)
        .PushBack(upperBodyRect.y, allocator)
        .PushBack(upperBodyRect.width, allocator)
        .PushBack(upperBodyRect.height, allocator);
    person.AddMember("upperBody", upperBody, allocator);
    person.AddMember("upperBody_score", getScore(NOSE, RIGHT_HIP), allocator);
    m_root["poses"].PushBack(person, allocator);

    return {{faceRect, bodyRect, upperBodyRect}};
}

cv::Rect2f PoseLandmarkDescriptor::getRect(
        LandmarkType beginType, LandmarkType endType) const
{
    float left, right, top, bottom;
    left = top = 1.0f;
    right = bottom = 0.0f;
    for (int type = beginType; type <= endType; type++) {
        left = std::min(left, m_landmarks[type][COOD_X]);
        right = std::max(right, m_landmarks[type][COOD_X]);
        top = std::min(top, m_landmarks[type][COOD_Y]);
        bottom = std::max(bottom, m_landmarks[type][COOD_Y]);
    }

    float width = right - left;
    float height = bottom - top;
    return cv::Rect2f(left, top, width, height);
}

float PoseLandmarkDescriptor::getScore(
        LandmarkType beginType, LandmarkType endType) const
{
    float score = 0;
    for (int type = beginType; type <= endType; type++) {
        score += m_landmarks[type][PRESENCE];
    }
    score = score / (endType - beginType + 1);
    return score;
}

}
