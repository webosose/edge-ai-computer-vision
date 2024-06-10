/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SERVER_H
#define SERVER_H

#include <WSServer.h>

class EdgeAIServer : public aif::WSServer
{
public:
    EdgeAIServer(asio::io_context& ioc, tcp::endpoint endpoint);

    ~EdgeAIServer() override;

protected:
    std::shared_ptr<aif::WSServerSession> createSession(tcp::socket&& socket) override;

};

#endif // end of SERVER_H
