#include <aif/pose/PosenetDescriptor.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>
#include <limits>

namespace {
} // anonymous namespace

namespace rj = rapidjson;

namespace aif {

PosenetDescriptor::PosenetDescriptor()
    : Descriptor()
{
//    edges.push_back(make_pair(KeyPointType::NOSE, KeyPointType::LEFT_EYE));
//    edges.push_back(make_pair(KeyPointType::NOSE, KeyPointType::RIGHT_EYE));
//    edges.push_back(make_pair(KeyPointType::NOSE, KeyPointType::LEFT_EAR));
//    edges.push_back(make_pair(KeyPointType::NOSE, KeyPointType::RIGHT_EAR));
//    edges.push_back(make_pair(KeyPointType::LEFT_EAR, KeyPointType::LEFT_EYE));
//    edges.push_back(make_pair(KeyPointType::RIGHT_EAR, KeyPointType::RIGHT_EYE));
//    edges.push_back(make_pair(KeyPointType::LEFT_EYE, KeyPointType::RIGHT_EYE));
//    edges.push_back(make_pair(KeyPointType::LEFT_SHOULDER, KeyPointType::RIGHT_SHOULDER));
//    edges.push_back(make_pair(KeyPointType::LEFT_SHOULDER, KeyPointType::LEFT_ELBOW));
//    edges.push_back(make_pair(KeyPointType::LEFT_SHOULDER, KeyPointType::LEFT_HIP));
//    edges.push_back(make_pair(KeyPointType::RIGHT_SHOULDER, KeyPointType::RIGHT_ELBOW));
//    edges.push_back(make_pair(KeyPointType::RIGHT_SHOULDER, KeyPointType::RIGHT_HIP));
//    edges.push_back(make_pair(KeyPointType::LEFT_ELBOW, KeyPointType::LEFT_WRIST));
//    edges.push_back(make_pair(KeyPointType::RIGHT_ELBOW, KeyPointType::RIGHT_WRIST));
//    edges.push_back(make_pair(KeyPointType::LEFT_HIP, KeyPointType::RIGHT_HIP));
//    edges.push_back(make_pair(KeyPointType::LEFT_HIP, KeyPointType::LEFT_KNEE));
//    edges.push_back(make_pair(KeyPointType::RIGHT_HIP, KeyPointType::RIGHT_KNEE));
//    edges.push_back(make_pair(KeyPointType::LEFT_KNEE, KeyPointType::LEFT_ANKLE));
//    edges.push_back(make_pair(KeyPointType::RIGHT_KNEE, KeyPointType::RIGHT_ANKLE));

    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    rj::Value poses(rj::kArrayType);
    m_root.AddMember("poses", poses, allocator);
}

PosenetDescriptor::~PosenetDescriptor()
{
}

void PosenetDescriptor::addKeyPoints( const std::vector<cv::Point>& points)
{
    m_keyPoints.push_back(points);
}

cv::Rect PosenetDescriptor::getRect(const std::vector<KeyPointType>& types,
        const std::vector<cv::Point>& keyPoints) const
{
    int left, right, top, bottom;
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
    return cv::Rect(x, y, width, height);
}

cv::Rect PosenetDescriptor::getFaceRect(const std::vector<cv::Point>& keyPoints) const {
    float scaleW = 1.2;
    float scaleH = 1.2;
    float width = (keyPoints[KeyPointType::LEFT_EAR].x - keyPoints[KeyPointType::RIGHT_EAR].x) * scaleW;
    float height = width * scaleH;
    float x = (keyPoints[KeyPointType::LEFT_EAR].x + keyPoints[KeyPointType::RIGHT_EAR].x) / 2.0 - (width / 2.0);
    float y = keyPoints[KeyPointType::NOSE].y - (height / 2.0);

    cv::Rect faceRect(x, y, width, height);
    return faceRect;
}

cv::Rect PosenetDescriptor::getUpperBodyRect(const std::vector<cv::Point>& keyPoints) const {
    std::vector<KeyPointType> types;
    for (int type = NOSE; type <= RIGHT_HIP; type++) {
        types.emplace_back(static_cast<KeyPointType>(type));
    }
    return getRect(types, keyPoints);
}

cv::Rect PosenetDescriptor::getBodyRect(const std::vector<cv::Point>& keyPoints) const {
    std::vector<KeyPointType> types;
    for (int type = NOSE; type <= RIGHT_ANKLE; type++) {
        types.emplace_back(static_cast<KeyPointType>(type));
    }
    return getRect(types, keyPoints);
}

void PosenetDescriptor::makeBodyParts()
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();

    for (auto keyPoints : m_keyPoints) {
        rj::Value person(rj::kObjectType);
        cv::Rect faceRect = getFaceRect(keyPoints);
        rj::Value face(rj::kArrayType);
        face.PushBack(faceRect.x, allocator)
            .PushBack(faceRect.y, allocator)
            .PushBack(faceRect.width, allocator)
            .PushBack(faceRect.height, allocator);
        person.AddMember("face", face, allocator);

        cv::Rect bodyRect = getBodyRect(keyPoints);
        rj::Value body(rj::kArrayType);
        body.PushBack(bodyRect.x, allocator)
            .PushBack(bodyRect.y, allocator)
            .PushBack(bodyRect.width, allocator)
            .PushBack(bodyRect.height, allocator);
        person.AddMember("body", body, allocator);

        cv::Rect upperBodyRect = getUpperBodyRect(keyPoints);
        rj::Value upperBody(rj::kArrayType);
        upperBody.PushBack(upperBodyRect.x, allocator)
            .PushBack(upperBodyRect.y, allocator)
            .PushBack(upperBodyRect.width, allocator)
            .PushBack(upperBodyRect.height, allocator);
        person.AddMember("upperBody", upperBody, allocator);
        m_root["poses"].PushBack(person, allocator);
    }
}

}

