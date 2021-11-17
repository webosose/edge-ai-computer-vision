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
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    rj::Value poses(rj::kArrayType);
    m_root.AddMember("poses", poses, allocator);
    rj::Value segments(rj::kArrayType);
    m_root.AddMember("segments", segments, allocator);
}

PoseLandmarkDescriptor::~PoseLandmarkDescriptor()
{
}

void PoseLandmarkDescriptor::addMaskData(int width, int height, float* mask)
{
    std::string maskData;
    int j = 0;
    for (int i = 0; i < width * height; i++) {
        if (sigmoid<float>(mask[j++]) > 0.1f){
            maskData.push_back('1');
        }
        else {
            maskData.push_back('0');
        }
    }
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    rj::Value segment(rj::kObjectType);
    segment.AddMember("width", width, allocator);
    segment.AddMember("height", height, allocator);
    segment.AddMember("mask", maskData, allocator);
    m_root["segments"].PushBack(segment, allocator);
}

void PoseLandmarkDescriptor::addLandmarks(float* landmarks)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    rj::Value person(rj::kObjectType);
    rj::Value points(rj::kArrayType);
    for (int i = 0; i < LANDMARK_TYPES::LANDMARK_TYPE_COUNT; i++) {
        rj::Value point(rj::kArrayType);
        for (int j = i * LANDMARK_ITEM_COUNT; j < (i+1) * LANDMARK_ITEM_COUNT; j++) {
            point.PushBack(landmarks[j], allocator);
        }
        points.PushBack(point, allocator);
    }
    person.AddMember("landmarks", points, allocator);
    m_root["poses"].PushBack(person, allocator);
}

}
