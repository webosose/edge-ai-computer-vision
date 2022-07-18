#include "Command.h"

#include <aif/log/Logger.h>

CommandResult::CommandResult(const std::string &name)
{
    m_doc.SetObject();
    m_doc.AddMember("cmd", name, m_doc.GetAllocator());
}

CommandResult::~CommandResult()
{
}

bool CommandResult::addReturnCode(aif::t_aif_status status)
{
    m_doc.AddMember("returnCode", status, m_doc.GetAllocator());
    return true;
}

template <typename T>
bool CommandResult::addResult(const ::std::string &resName, T &res)
{
    m_doc.AddMember(resName, res, m_doc.GetAllocator());
    return true;
}

std::string CommandResult::toStr() const
{
    rj::StringBuffer buffer;
    rj::Writer<rj::StringBuffer> writer(buffer);
    m_doc.Accept(writer);
    return buffer.GetString();
}

Command::Command(aif::EdgeAIVision &ai, const std::string &name)
    : m_ai(ai), m_result(name)
{
}

Command::~Command()
{
}