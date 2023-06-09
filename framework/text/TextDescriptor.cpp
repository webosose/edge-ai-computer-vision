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
    rj::Value texts(rj::kObjectType);
    m_root.AddMember("texts", texts, allocator);
}

TextDescriptor::~TextDescriptor()
{
}

bool TextDescriptor::addBboxes(const std::vector<cv::Rect>& bboxes)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("texts")) {
        rj::Value texts(rj::kObjectType);
        m_root.AddMember("texts", texts, allocator);
        m_textRectCount = 0;
    }

    rj::Value bbox(rj::kObjectType);
    rj::Value rects(rj::kArrayType);
    for (const auto& rect : bboxes) {
        rj::Value region(rj::kArrayType);
        region.PushBack(rect.x, allocator)
            .PushBack(rect.y, allocator)
            .PushBack(rect.width, allocator)
            .PushBack(rect.height, allocator);
        rects.PushBack(region, allocator);
    }

    m_root["texts"].AddMember("bbox", rects, allocator);

    TRACE("rect count : ", bboxes.size());
    m_textRectCount += bboxes.size();
    return true;
}

bool TextDescriptor::addBoxes(const std::vector<std::vector<cv::Point>>& boxes)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("texts")) {
        rj::Value texts(rj::kObjectType);
        m_root.AddMember("texts", texts, allocator);
    }

    rj::Value box(rj::kObjectType);
    rj::Value points(rj::kArrayType);
    for (const auto& pts : boxes) {
        if (pts.size() < 4) continue;
        rj::Value point(rj::kArrayType);
        for (const auto& pt : pts) {
            point.PushBack(pt.x, allocator)
                 .PushBack(pt.y, allocator);
        }
        points.PushBack(point, allocator);
    }
    m_root["texts"].AddMember("box", points, allocator);

    return true;
}


}
