/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/palm/PalmDescriptor.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>
#include <limits>

namespace {
} // anonymous namespace

namespace rj = rapidjson;

namespace aif {

PalmDescriptor::PalmDescriptor()
    : Descriptor()
    , m_palmCount(0)
{
}

PalmDescriptor::~PalmDescriptor()
{
}

void PalmDescriptor::addPalm(float score, const std::vector<float>& palmPoints)
{
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("palms")) {
        rj::Value palms(rj::kArrayType);
        m_root.AddMember("palms", palms, allocator);
    }

    rj::Value palm(rj::kObjectType);
    palm.AddMember("score", score, allocator);

    // region: [x, y, w, h]
    rj::Value region(rj::kArrayType);
    region.PushBack(palmPoints[0], allocator);
    region.PushBack(palmPoints[1], allocator);
    region.PushBack(palmPoints[2] - palmPoints[0], allocator);
    region.PushBack(palmPoints[3] - palmPoints[1], allocator);
    palm.AddMember("region", region, allocator);

    rj::Value keyPoints(rj::kArrayType);
    for (int i = 4; i < 18; i++) {
        keyPoints.PushBack(palmPoints[i], allocator);
    }
    palm.AddMember("keyPoints", keyPoints, allocator);

    m_root["palms"].PushBack(palm, allocator);
    m_palmCount++;
}

void PalmDescriptor::clear()
{
    m_palmCount = 0;
    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    m_root.RemoveMember("palms");
    rj::Value palms(rj::kArrayType);
    m_root.AddMember("palms", palms, allocator);
}


}
