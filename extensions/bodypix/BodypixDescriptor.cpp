/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/bodypix/BodypixDescriptor.h>
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
}

BodypixDescriptor::~BodypixDescriptor()
{
}

void BodypixDescriptor::addMaskData(int width, int height, uint8_t* mask)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("segments")) {
        rj::Value segments(rj::kArrayType);
        m_root.AddMember("segments", segments, allocator);
    }
    rj::Value segment(rj::kObjectType);
    rj::Value maskData(rj::kArrayType);
    int maskSize = width * height;
    for (int i = 0; i < maskSize; i++) {
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
