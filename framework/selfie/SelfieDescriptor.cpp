#include <aif/selfie/SelfieDetector.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>
#include <limits>

namespace {
} // anonymous namespace

namespace rj = rapidjson;

namespace aif {

SelfieDescriptor::SelfieDescriptor()
    : Descriptor()
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    rj::Value segments(rj::kArrayType);
    m_root.AddMember("segments", segments, allocator);
}

SelfieDescriptor::~SelfieDescriptor()
{
}

void SelfieDescriptor::addMaskData(int width, int height, float* mask)
{
    std::string maskData;
    int j = 0;
    for (int i = 0; i < width * height; i++) {
        if (mask[j++] > 0.9f){
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


}
