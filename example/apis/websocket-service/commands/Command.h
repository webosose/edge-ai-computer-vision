#ifndef AIF_COMMAND_H
#define AIF_COMMAND_H

#include <aif/facade/EdgeAIVision.h>
#include <aif/base/Types.h>

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <string>

namespace rj = rapidjson;

class CommandResult
{
public:
    CommandResult(const std::string &name);
    virtual ~CommandResult();

    template <typename T>
    bool addResult(const ::std::string &resName, T &res);
    bool addReturnCode(aif::t_aif_status status);
    std::string toStr() const;

private:
    rj::Document m_doc;
};

class Command
{
public:
    Command(aif::EdgeAIVision &ai, const std::string &name);
    virtual ~Command();
    virtual bool execute(const rj::Document &request, std::string &response) = 0;

protected:
    aif::EdgeAIVision &m_ai;
    CommandResult m_result;
};

#endif // AIF_COMMAND_H
