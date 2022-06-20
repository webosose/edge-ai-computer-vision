/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FACE_WSSERVER_H
#define FACE_WSSERVER_H

#include <aif/base/WSServer.h>

class FaceWSServer : public aif::WSServer
{
public:
    FaceWSServer(asio::io_context& ioc, tcp::endpoint endpoint);

    ~FaceWSServer() override;

protected:
    std::shared_ptr<aif::WSServerSession> createSession(tcp::socket&& socket) override;

};

#endif // end of FACE_WSSERVER_H