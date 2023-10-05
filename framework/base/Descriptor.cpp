/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/Descriptor.h>
#include <aif/log/Logger.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>

namespace rj = rapidjson;

namespace aif {

Descriptor::Descriptor()
{
    m_root.SetObject();
}

Descriptor::~Descriptor()
{
}

void Descriptor::addResponseName(const std::string& responseName, const std::string id)
{
    m_root.AddMember("response", responseName, m_root.GetAllocator());
    m_root.AddMember("id", id, m_root.GetAllocator());
}

void Descriptor::addReturnCode(int returnCode)
{
    m_root.AddMember("returnCode", returnCode, m_root.GetAllocator());
}


std::string Descriptor::toStr()
{
    rj::StringBuffer buffer;
    rj::Writer<rj::StringBuffer> writer(buffer);
    writer.SetMaxDecimalPlaces(4);
    m_root.Accept(writer);
    return buffer.GetString();
}

bool Descriptor::hasMember(const std::string& member) const
{
    return m_root.HasMember(member);
}

void Descriptor::initExtraOutput(const ExtraOutput& extraOutput)
{
    m_extraOutput = extraOutput;
    m_root.AddMember("useExtraOutput", true, m_root.GetAllocator());
}

const ExtraOutput& Descriptor::getExtraOutput() const
{
    if (!m_root.HasMember("useExtraOutput")) {
        Loge("useExtraOutput is false.");
    }
    return m_extraOutput;
}

bool Descriptor::addExtraOutput(ExtraOutputType type, void* buffer, size_t size)
{
    if (m_extraOutput.type() != type) {
        Loge("extra output buffer type is not same");
        Loge("extra buffer type: ", static_cast<int>(m_extraOutput.type()));
        Loge("data  buffer type: ", static_cast<int>(type));
        return false;
    }
    if (m_extraOutput.buffer() == nullptr) {
        Loge("extra output buffer ptr is null");
        return false;
    }
    if (m_extraOutput.bytes() < size) {
        Loge("extra output buffer size is less than data buffer size");
        return false;
    }
    memcpy(m_extraOutput.buffer(), buffer, size);
    return true;
}

} // end of namespace aif
