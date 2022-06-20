/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_WSSERVER_H
#define AIF_WSSERVER_H

#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <aif/base/WSServerSession.h>

namespace aif {

class WSServer : public std::enable_shared_from_this<WSServer>
{
public:
    WSServer(asio::io_context& ioc,
             asio::ip::tcp::endpoint endpoint);

    virtual ~WSServer();

    // Start accepting incoming connections
    void run();

protected:
    void accept();

    void onAccept(beast::error_code ec,
                  tcp::socket socket);

    // Report a failure
    void fail(beast::error_code ec, char const* what);

    virtual std::shared_ptr<WSServerSession> createSession(tcp::socket&& socket);

protected:
    asio::io_context& m_ioc;
    tcp::acceptor m_acceptor;
};

} // end of namespace aif

#endif // AIF_WSSERVER_H
