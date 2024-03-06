/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/semantic/SemanticDetector.h>
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

SemanticDescriptor::SemanticDescriptor()
    : Descriptor()
{
}

SemanticDescriptor::~SemanticDescriptor()
{
}

void SemanticDescriptor::addMaskData(int width, int height, int64_t* mask)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("segments")) {
        rj::Value segments(rj::kArrayType);
        m_root.AddMember("segments", segments, allocator);
    }

    int j = 0;
    rj::Value segment(rj::kObjectType);
    rj::Value maskData(rj::kArrayType);
    for (int i = 0; i < CHECK_INT_MUL(width, height); i++) {
        if (mask[j++] == 15)
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
