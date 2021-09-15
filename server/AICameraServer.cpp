#include "AICameraServer.h"
#include "AICameraServerSession.h"

//--------------------------------------------------------------
// AICameraServer class definition
//--------------------------------------------------------------
AICameraServer::AICameraServer(asio::io_context& ioc, tcp::endpoint endpoint)
    : aif::WSServer(ioc, endpoint)
{
}

AICameraServer::~AICameraServer()/* override*/
{
}

std::shared_ptr<aif::WSServerSession> AICameraServer::createSession(tcp::socket&& socket)/* override*/
{
    return std::make_shared<AICameraServerSession>(std::move(socket));
}
