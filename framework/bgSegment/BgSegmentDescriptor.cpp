/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bgSegment/BgSegmentDetector.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>
#include <limits>

namespace {
} // anonymous namespace

namespace rj = rapidjson;

namespace aif {

BgSegmentDescriptor::BgSegmentDescriptor()
    : Descriptor()
{
}

BgSegmentDescriptor::~BgSegmentDescriptor()
{
}

void BgSegmentDescriptor::addMaskInfo(int x, int y, int width, int height, int tensorW, int tensorH)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("segments")) {
        rj::Value segments(rj::kArrayType);
        m_root.AddMember("segments", segments, allocator);
    }

    rj::Value segment(rj::kObjectType);

    rj::Value inputRect(rj::kArrayType);
    inputRect.PushBack(x, allocator);
    inputRect.PushBack(y, allocator);
    inputRect.PushBack(width, allocator);
    inputRect.PushBack(height, allocator);
    segment.AddMember("inputRect", inputRect, allocator);

    rj::Value maskRect(rj::kArrayType);
    maskRect.PushBack(tensorW, allocator);
    maskRect.PushBack(tensorH, allocator);
    segment.AddMember("maskRect", maskRect, allocator);

    m_root["segments"].PushBack(segment, allocator);

/*
    segment.AddMember("x", x, allocator);
    segment.AddMember("y", y, allocator);
    segment.AddMember("width", width, allocator);
    segment.AddMember("height", height, allocator);
    m_root["segments"].PushBack(segment, allocator);*/
}

}
