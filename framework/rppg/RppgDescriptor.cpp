/*
* Copyright (c) 2023 LG Electronics Inc.
* SPDX-License-Identifier: Apache-2.0
*/

#include <aif/rppg/RppgDescriptor.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace rj = rapidjson;

namespace aif {

RppgDescriptor::RppgDescriptor()
: Descriptor()
{
}

RppgDescriptor::~RppgDescriptor()
{
}

void RppgDescriptor::addRppgOutput(float bpm, std::string signalCondition)
{
    rj::Document::AllocatorType &allocator = m_root.GetAllocator();
    if (!m_root.HasMember("rPPG"))
    {
        rj::Value rPPG(rj::kArrayType);
        m_root.AddMember("rPPG", rPPG, allocator);
    }
    rj::Value data(rj::kObjectType);
    // add signal condition and bpm
    data.AddMember("signalCondition", signalCondition, allocator);
    data.AddMember("bpm", bpm, allocator);
    m_root["rPPG"].PushBack(data, allocator);
}

void RppgDescriptor::clear()
{
    rj::Document::AllocatorType &allocator = m_root.GetAllocator();
    m_root.RemoveMember("rPPG");
    rj::Value rPPG(rj::kArrayType);
    m_root.AddMember("rPPG", rPPG, allocator);
}

} // end of namespace aif
