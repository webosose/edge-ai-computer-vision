#ifndef AIF_WSCLIENT_H
#define AIF_WSCLIENT_H

#include <aif/base/Types.h>

#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <functional>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp
namespace asio = boost::asio;           // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace aif {

class WSClient : public std::enable_shared_from_this<WSClient>
{
public:
    using OpenCallback = std::function<void(const t_aif_event& e)>;
    using CloseCallback = std::function<void(const t_aif_event& e)>;
    using MessageCallback = std::function<void(const t_aif_event& e)>;
    using ErrorCallback = std::function<void(const t_aif_event& e)>;

    WSClient(
        asio::io_context& ioc,
        ErrorCallback errorCb = nullptr);

    virtual ~WSClient();

    void open(
        const std::string& host,
        const std::string& port,
        OpenCallback openCb = nullptr);

    void sendMessage(
        const std::string& message,
        MessageCallback messageCb = nullptr);

    void close(
        CloseCallback closeCb = nullptr);

protected:
    void onResolve(
        beast::error_code ec,
        tcp::resolver::results_type results);

    void onConnect(
        beast::error_code ec,
        tcp::resolver::results_type::endpoint_type);

    void onOpen(
        beast::error_code ec);

    void onSend(
        beast::error_code ec,
        std::size_t bytes_transferred);

    void onMessage(
        beast::error_code ec,
        std::size_t bytes_transferred);

    void onClose(
        beast::error_code ec);

protected:
    tcp::resolver m_resolver;
    websocket::stream<beast::tcp_stream> m_ws;
    beast::flat_buffer m_buffer;
    std::string m_host;

    ErrorCallback m_errorCb;
    OpenCallback m_openCb;
    CloseCallback m_closeCb;
    MessageCallback m_messageCb;
};

} // end of namespace aif

#endif // AIF_WSCLIENT_H
