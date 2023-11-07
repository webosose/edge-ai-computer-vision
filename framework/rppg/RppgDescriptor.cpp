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
, m_rppgOut()
, m_batchSize(0)
, m_channelSize(0)
{
}

RppgDescriptor::~RppgDescriptor()
{
}

void RppgDescriptor::addRppgOutput(std::vector<float> outputs)
{
    rj::Document::AllocatorType &allocator = m_root.GetAllocator();
    if (!m_root.HasMember("rPPG")) {
        rj::Value rPPG(rj::kArrayType);
        m_root.AddMember("rPPG", rPPG, allocator);
    }

    rj::Value data(rj::kObjectType);
    for (int i = 0; i < outputs.size(); i++) {
        data.PushBack(outputs[i], allocator);
    }
    data.AddMember("outputTensors", data, allocator);
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
