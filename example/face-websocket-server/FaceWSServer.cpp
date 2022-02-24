#include "FaceWSServer.h"
#include "FaceWSServerSession.h"

//--------------------------------------------------------------
// FaceWSServer class definition
//--------------------------------------------------------------
FaceWSServer::FaceWSServer(asio::io_context& ioc, tcp::endpoint endpoint)
    : aif::WSServer(ioc, endpoint)
{
}

FaceWSServer::~FaceWSServer()/* override*/
{
}

std::shared_ptr<aif::WSServerSession> FaceWSServer::createSession(tcp::socket&& socket)/* override*/
{
    return std::make_shared<FaceWSServerSession>(std::move(socket));
}
