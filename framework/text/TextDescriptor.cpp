/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/text/TextDescriptor.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>
#include <limits>

namespace {
} // anonymous namespace

namespace rj = rapidjson;

namespace aif {

TextDescriptor::TextDescriptor()
    : Descriptor()
    , m_textRectCount(0)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    rj::Value texts(rj::kArrayType);
    m_root.AddMember("texts", texts, allocator);
}

TextDescriptor::~TextDescriptor()
{
}

bool TextDescriptor::addTextRects(const std::vector<cv::Rect>& rects)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("texts")) {
        rj::Value texts(rj::kArrayType);
        m_root.AddMember("texts", texts, allocator);
        m_textRectCount = 0;
    }

    for (const auto& rect : rects) {
        rj::Value region(rj::kArrayType);
        region.PushBack(rect.x, allocator)
            .PushBack(rect.y, allocator)
            .PushBack(rect.width, allocator)
            .PushBack(rect.height, allocator);
        m_root["texts"].PushBack(region, allocator);
    }

    TRACE("rect count : ", rects.size());
    m_textRectCount += rects.size();
    return true;
}

}
