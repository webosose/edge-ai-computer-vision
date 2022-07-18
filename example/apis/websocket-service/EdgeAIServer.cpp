#include "EdgeAIServer.h"
#include "EdgeAIServerSession.h"

//--------------------------------------------------------------
// EdgeAIServer class definition
//--------------------------------------------------------------
EdgeAIServer::EdgeAIServer(asio::io_context& ioc, tcp::endpoint endpoint)
    : aif::WSServer(ioc, endpoint)
{
}

EdgeAIServer::~EdgeAIServer()/* override*/
{
}

std::shared_ptr<aif::WSServerSession> EdgeAIServer::createSession(tcp::socket&& socket)/* override*/
{
    return std::make_shared<EdgeAIServerSession>(std::move(socket));
}
