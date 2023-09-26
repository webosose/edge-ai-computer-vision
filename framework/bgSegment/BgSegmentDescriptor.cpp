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

void BgSegmentDescriptor::addMaskInfo(int x, int y, int width, int height)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("segments")) {
        rj::Value segments(rj::kArrayType);
        m_root.AddMember("segments", segments, allocator);
    }

    rj::Value segment(rj::kObjectType);

    segment.AddMember("x", x, allocator);
    segment.AddMember("y", y, allocator);
    segment.AddMember("width", width, allocator);
    segment.AddMember("height", height, allocator);
    m_root["segments"].PushBack(segment, allocator);
}

}
