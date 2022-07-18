#include "EdgeAIServerSession.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <aif/base/DelegateFactory.h>
#include <aif/base/DetectorFactory.h>
#include <aif/tools/Stopwatch.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include "commands/CommandFactory.h"

using namespace aif;
namespace rj = rapidjson;

//--------------------------------------------------------------
// EdgeAIServerSession class definition
//--------------------------------------------------------------

EdgeAIServerSession::EdgeAIServerSession(tcp::socket &&socket)
    : WSServerSession(std::move(socket)), m_ai(EdgeAIVision::getInstance())
{
    Logi("[begin session]");
}

EdgeAIServerSession::~EdgeAIServerSession() /* override*/
{
    Logi("[end session]");
}

void EdgeAIServerSession::onInit() /* override*/
{
}

void EdgeAIServerSession::onHandleMessage(const std::string &message) /* override*/
{
    std::string commandName;
    try
    {
        rj::Document request;
        request.Parse(message.c_str());
        commandName = request["cmd"].GetString();
        Logi("Command: ", commandName);

        std::string res;
        if (CommandFactory::get().create(m_ai, commandName)->execute(request, res))
        {
            response(res);
        }
        else
        {
            onHandleError(commandName, "failed to execute a request");
        }
    }
    catch (beast::system_error const &se)
    {
        if (se.code() == websocket::error::closed)
        {
            Logi(__func__, "[", se.code(), "] ", se.code().message());
        }
        else
        {
            Loge(__func__, "Error: [", se.code(), "] ", se.code().message());
        }
        onHandleError(commandName, se.code().message());
    }
    catch (const std::exception &e)
    {
        Loge(__func__, "Error: ", e.what());
        onHandleError(commandName, e.what());
    }
}

void EdgeAIServerSession::onHandleError(
    const std::string &req, const std::string &errmsg)
{
    rj::Document res;
    res.SetObject();
    res.AddMember("returnCode", kAifError, res.GetAllocator());
    res.AddMember("req", req, res.GetAllocator());
    res.AddMember("error", errmsg, res.GetAllocator());

    rj::StringBuffer buffer;
    rj::Writer<rj::StringBuffer> writer(buffer);
    res.Accept(writer);

    response(buffer.GetString());
}