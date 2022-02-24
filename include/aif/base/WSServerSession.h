#ifndef AIF_WSSERVER_SESSION_H
#define AIF_WSSERVER_SESSION_H

#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <map>

#include <aif/base/Types.h>
#include <msgpack.hpp>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp
namespace asio = boost::asio;           // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace aif {

class WSServerSession : public std::enable_shared_from_this<WSServerSession>
{
public:
    // Take ownership of the socket
    explicit WSServerSession(
        tcp::socket&& socket);

    virtual ~WSServerSession();

    // Get on the correct executor
    void run();

protected:

    virtual void onInit();

    // Start the asynchronous operation
    void onRun();

    void onAccept(
        beast::error_code ec);

    void readMessage();

    void onMessage(
        beast::error_code ec,
        std::size_t bytes_transferred);

    virtual void onHandleMessage(
        const std::string& message);

    void response(const std::string& message);

    void onResponse(
        beast::error_code ec,
        std::size_t bytes_transferred);

    // Report a failure
    void fail(beast::error_code ec, char const* what);

protected:
    beast::websocket::stream<beast::tcp_stream> m_ws;
    beast::flat_buffer m_buffer;
    std::shared_ptr<msgpack::sbuffer> m_outBuffer;
};

} // end of namespace aif

#endif // AIF_WSSERVER_SESSION_H
