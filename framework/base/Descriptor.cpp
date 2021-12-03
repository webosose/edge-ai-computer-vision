#include <aif/base/Descriptor.h>

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
    m_root.Accept(writer);
    return buffer.GetString();
}

} // end of namespace aif
