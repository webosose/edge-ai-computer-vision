/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

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
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    rj::Value poses(rj::kArrayType);
    m_root.AddMember("poses", poses, allocator);
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

cv::Rect2f PosenetDescriptor::getRect(
        const std::vector<cv::Point2f>& keyPoints,
        KeyPointType beginType,
        KeyPointType endType)  const
{
    float left, right, top, bottom;
    left = top = 1.0f;
    right = bottom = 0.0f;
    for (int type = beginType; type <= endType; type++) {
        left = std::min(left, keyPoints[type].x);
        right = std::max(right, keyPoints[type].x);
        top = std::min(top, keyPoints[type].y);
        bottom = std::max(bottom, keyPoints[type].y);
    }

    float width = right - left;
    float height = bottom - top;
    return cv::Rect2f(left, top, width, height);
}

float PosenetDescriptor::getScore(
        const std::vector<float>& scores,
        KeyPointType begin,
        KeyPointType end) const {
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
    if (!m_keyPoints.size()) {
        return {{ cv::Rect2f(0,0,0,0) }};
    }
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("poses")) {
        rj::Value poses(rj::kArrayType);
        m_root.AddMember("poses", poses, allocator);
    }

    TRACE("keypoint size: " , m_keyPoints.size());
    std::vector<std::vector<cv::Rect2f>> current;
    for (int i = 0; i < m_keyPoints.size(); i++) {
        auto& keyPoints = m_keyPoints[i];
        auto& scores = m_keyPointsScores[i];

        rj::Value person(rj::kObjectType);
        cv::Rect2f faceRect = getRect(keyPoints, NOSE, RIGHT_EAR);
        if (i < prev.size() && isIOUOver(faceRect, prev[i][0], iouThreshold)) {
            faceRect = prev[i][0];
        }
        rj::Value face(rj::kObjectType);
        rj::Value faceRegion(rj::kArrayType);
        faceRegion.PushBack(faceRect.x, allocator)
            .PushBack(faceRect.y, allocator)
            .PushBack(faceRect.width, allocator)
            .PushBack(faceRect.height, allocator);
        face.AddMember("region", faceRegion, allocator);
        face.AddMember("score", getScore(scores, NOSE, RIGHT_EAR), allocator);
        person.AddMember("face", face, allocator);

        cv::Rect2f bodyRect = getRect(keyPoints, NOSE, RIGHT_ANKLE);
        if (i < prev.size() && isIOUOver(bodyRect, prev[i][1], iouThreshold)) {
            bodyRect = prev[i][1];
        }
        rj::Value body(rj::kObjectType);
        rj::Value bodyRegion(rj::kArrayType);
        bodyRegion.PushBack(bodyRect.x, allocator)
            .PushBack(bodyRect.y, allocator)
            .PushBack(bodyRect.width, allocator)
            .PushBack(bodyRect.height, allocator);
        body.AddMember("region", bodyRegion, allocator);
        body.AddMember("score", getScore(scores, NOSE, RIGHT_ANKLE), allocator);
        person.AddMember("body", body, allocator);

        cv::Rect2f upperBodyRect = getRect(keyPoints, NOSE, RIGHT_HIP);
        if (i < prev.size() && isIOUOver(upperBodyRect, prev[i][2], iouThreshold)) {
            upperBodyRect = prev[i][2];
        }
        rj::Value upperBody(rj::kObjectType);
        rj::Value upperRegion(rj::kArrayType);
        upperRegion.PushBack(upperBodyRect.x, allocator)
            .PushBack(upperBodyRect.y, allocator)
            .PushBack(upperBodyRect.width, allocator)
            .PushBack(upperBodyRect.height, allocator);
        upperBody.AddMember("region", upperRegion, allocator);
        upperBody.AddMember("score", getScore(scores, NOSE, RIGHT_HIP), allocator);
        person.AddMember("upperBody", upperBody, allocator);

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
