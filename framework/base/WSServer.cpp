#include <aif/base/WSServer.h>
#include <aif/log/Logger.h>

namespace aif {

WSServer::WSServer(asio::io_context& ioc, tcp::endpoint endpoint)
    : m_ioc(ioc)
    , m_acceptor(ioc)
{
    beast::error_code ec;

    // Open the acceptor
    m_acceptor.open(endpoint.protocol(), ec);
    if (ec) {
        fail(ec, "open");
        return;
    }

    // Allow address reuse
    m_acceptor.set_option(asio::socket_base::reuse_address(true), ec);
    if (ec) {
        fail(ec, "set_option");
        return;
    }

    // Bind to the server address
    m_acceptor.bind(endpoint, ec);
    if (ec) {
        fail(ec, "bind");
        return;
    }

    // Start listening for connections
    m_acceptor.listen(asio::socket_base::max_listen_connections, ec);
    if (ec) {
        fail(ec, "listen");
        return;
    }
}

WSServer::~WSServer()
{
}

// Start accepting incoming connections
void WSServer::run()
{
    accept();
}

void WSServer::accept()
{
    // The new connection gets its own strand
    m_acceptor.async_accept(
        asio::make_strand(m_ioc),
        beast::bind_front_handler(
            &WSServer::onAccept,
            shared_from_this()));
}

void WSServer::onAccept(beast::error_code ec, tcp::socket socket)
{
    try {
        if (ec) throw beast::system_error(ec);

        // Create the session and run it
        auto session = createSession(std::move(socket));
        if (session == nullptr) {
            throw std::runtime_error("make session failed!");
        }

        session->run();

        // Accept another connection
        accept();
    } catch(beast::system_error const& se) {
        if(se.code() == websocket::error::closed) {
            Logi("connection closed.");
        } else {
            Loge(__func__, "Error: [", se.code() , "] ", se.code().message());
        }
    } catch(const std::exception& e) {
        Loge(__func__, "Error: ", e.what());
    }
}

/*virtual */
std::shared_ptr<WSServerSession> WSServer::createSession(tcp::socket&& socket)
{
    return std::make_shared<WSServerSession>(std::move(socket));
}

// Report a failure
void WSServer::fail(beast::error_code ec, char const* what)
{
    AIF_ERROR << what << ": " << ec.message();
}

} // end of namespace aif
