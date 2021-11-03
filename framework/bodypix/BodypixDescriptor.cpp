#include <aif/bodypix/BodypixDescriptor.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>

namespace {
} // anonymous namespace

namespace rj = rapidjson;

namespace aif {

BodypixDescriptor::BodypixDescriptor()
    : Descriptor()
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    rj::Value segments(rj::kArrayType);
    m_root.AddMember("segments", segments, allocator);
}

BodypixDescriptor::~BodypixDescriptor()
{
}

void BodypixDescriptor::addMaskData(int width, int height, uint8_t* mask)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    rj::Value segment(rj::kObjectType);
    rj::Value maskData(rj::kArrayType);
    for (int i = 0; i < width * height; i++) {
        int q = (int)mask[i];
        float t = 0.08946539461612701 * (q - 160);
        if (t > 0.2)
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
