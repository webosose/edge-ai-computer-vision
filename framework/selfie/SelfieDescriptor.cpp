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
}

SelfieDescriptor::~SelfieDescriptor()
{
}

void SelfieDescriptor::addMaskData(int width, int height, float* mask)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("segments")) {
        rj::Value segments(rj::kArrayType);
        m_root.AddMember("segments", segments, allocator);

    }
    
    int j = 0;
    rj::Value segment(rj::kObjectType);
    rj::Value maskData(rj::kArrayType);
    for (int i = 0; i < width * height; i++) {
        if (mask[j++] > 0.9f)
            maskData.PushBack(1, allocator);
        else
            maskData.PushBack(0, allocator);
    }

    segment.AddMember("width", width, allocator);
    segment.AddMember("height", height, allocator);
    segment.AddMember("mask", maskData, allocator);
    m_root["segments"].PushBack(segment, allocator);
}


}
