#ifndef AICAMERA_SERVER_H
#define AICAMERA_SERVER_H

#include <aif/base/WSServer.h>

class AICameraServer : public aif::WSServer
{
public:
    AICameraServer(asio::io_context& ioc, tcp::endpoint endpoint);

    ~AICameraServer() override;

protected:
    std::shared_ptr<aif::WSServerSession> createSession(tcp::socket&& socket) override;

};

#endif // end of AICAMERA_SERVER_H