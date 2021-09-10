#include <aif/base/WSClient.h>
#include <aif/log/Logger.h>

#include <msgpack.hpp>

// #define RAPIDJSON_HAS_STDSTRING 1
// #include <rapidjson/document.h>
// #include <rapidjson/writer.h>
// #include <rapidjson/stringbuffer.h>

namespace aif {

// Resolver and socket require an io_context
WSClient::WSClient(
        asio::io_context& ioc,
        ErrorCallback errorCb)
    : m_resolver(asio::make_strand(ioc))
    , m_ws(asio::make_strand(ioc))
    , m_errorCb(errorCb)
    , m_openCb(nullptr)
    , m_closeCb(nullptr)
    , m_messageCb(nullptr)
{
}

WSClient::~WSClient()
{
}

void WSClient::open(
        const std::string& host,
        const std::string& port,
        OpenCallback openCb)
{
    // Save these for later
    m_host = host;

    if (openCb != nullptr)
        m_openCb = openCb;

    // Look up the domain name
    m_resolver.async_resolve(
        host,
        port,
        beast::bind_front_handler(
            &WSClient::onResolve,
            shared_from_this()));
}

void WSClient::onResolve(
    beast::error_code ec,
    tcp::resolver::results_type results)
{
    try {
        if (ec) throw beast::system_error(ec);

        // Set the timeout for the operation
        beast::get_lowest_layer(m_ws).expires_after(std::chrono::seconds(30));

        // Make the connection on the IP address we get from a lookup
        beast::get_lowest_layer(m_ws).async_connect(
            results,
            beast::bind_front_handler(
                &WSClient::onConnect,
                shared_from_this()));

    } catch(beast::system_error const& se) {
        Loge(__func__, "Error: [", se.code() , "] ", se.code().message());
        if (m_errorCb) {
            t_aif_event wse;
            wse.type = kWebSocketOpen;
            wse.data = se.code().message();
            m_errorCb(wse);
        }
    } catch(const std::exception& e) {
        Loge(__func__, "Error: ", e.what());
        if (m_errorCb) {
            t_aif_event wse;
            wse.type = kWebSocketOpen;
            wse.data = e.what();
            m_errorCb(wse);
        }
    }
}

void WSClient::onConnect(
    beast::error_code ec,
    tcp::resolver::results_type::endpoint_type)
{
    try {
        if (ec) throw beast::system_error(ec);

        // Turn off the timeout on the tcp_stream, because
        // the websocket stream has its own timeout system.
        beast::get_lowest_layer(m_ws).expires_never();

        // Set suggested timeout settings for the websocket
        m_ws.set_option(
            websocket::stream_base::timeout::suggested(
                beast::role_type::client));

        // Set a decorator to change the User-Agent of the handshake
        m_ws.set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req)
            {
                req.set(http::field::user_agent,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                        " aif-websocket-client");
            }));

        // Perform the websocket handshake
        m_ws.async_handshake(m_host, "/",
            beast::bind_front_handler(
                &WSClient::onOpen,
                shared_from_this()));

    } catch(beast::system_error const& se) {
        Loge(__func__, "Error: [", se.code() , "] ", se.code().message());
        if (m_errorCb) {
            t_aif_event wse;
            wse.type = kWebSocketOpen;
            wse.data = se.code().message();
            m_errorCb(wse);
        }
    } catch(const std::exception& e) {
        Loge(__func__, "Error: ", e.what());
        if (m_errorCb) {
            t_aif_event wse;
            wse.type = kWebSocketOpen;
            wse.data = e.what();
            m_errorCb(wse);
        }
    }
}

void WSClient::onOpen(beast::error_code ec)
{
    try {
        if (ec) throw beast::system_error(ec);

        TRACE("connection opened.");
        if (m_openCb) {
            t_aif_event wse;
            wse.type = kWebSocketOpen;
            wse.data = "connection opened";
            m_openCb(wse);
        }

    } catch(beast::system_error const& se) {
        Loge(__func__, "Error: [", se.code() , "] ", se.code().message());
        if (m_errorCb) {
            t_aif_event wse;
            wse.type = kWebSocketOpen;
            wse.data = se.code().message();
            m_errorCb(wse);
        }
    } catch(const std::exception& e) {
        Loge(__func__, "Error: ", e.what());
        if (m_errorCb) {
            t_aif_event wse;
            wse.type = kWebSocketOpen;
            wse.data = e.what();
            m_errorCb(wse);
        }
    }
}

void WSClient::sendMessage(
    const std::string& message,
    MessageCallback messageCb)
{
    // Save these for later
    if (messageCb != nullptr)
        m_messageCb = messageCb;

    // encode message
    std::stringstream out_buffer;
    msgpack::pack(out_buffer, message);
    out_buffer.seekg(0);

    // Send the message
    m_ws.binary(true);
    m_ws.async_write(
        asio::buffer(out_buffer.str()),
        beast::bind_front_handler(
            &WSClient::onSend,
            shared_from_this()));
}

void WSClient::onSend(
        beast::error_code ec,
        std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    try {
        if (ec) throw beast::system_error(ec);

        // read response message
        m_ws.binary(true);
        m_ws.async_read(
            m_buffer,
            beast::bind_front_handler(
                &WSClient::onMessage,
                shared_from_this()));

    } catch(beast::system_error const& se) {
        Loge(__func__, "Error: [", se.code() , "] ", se.code().message());
        if (m_errorCb) {
            t_aif_event wse;
            wse.type = kWebSocketMessage;
            wse.data = se.code().message();
            m_errorCb(wse);
        }
    } catch(const std::exception& e) {
        Loge(__func__, "Error: ", e.what());
        if (m_errorCb) {
            t_aif_event wse;
            wse.type = kWebSocketMessage;
            wse.data = e.what();
            m_errorCb(wse);
        }
    }
}

void WSClient::onMessage(
    beast::error_code ec,
    std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    try {
        if (ec) throw beast::system_error(ec);

        // decode message
        msgpack::object_handle oh = msgpack::unpack(
            boost::asio::buffer_cast<char*>(m_buffer.data()), m_buffer.size());
        msgpack::object deserialized = oh.get();

        std::string message;
        deserialized.convert(message);

        TRACE("message received");
        if (m_messageCb) {
            t_aif_event wse;
            wse.type = kWebSocketMessage;
            wse.data = message;
            m_messageCb(wse);
        }

        // Clear the buffer
        m_buffer.consume(m_buffer.size());

    } catch(beast::system_error const& se) {
        Loge(__func__, "Error: [", se.code() , "] ", se.code().message());
        if (m_errorCb) {
            t_aif_event wse;
            wse.type = kWebSocketMessage;
            wse.data = se.code().message();
            m_errorCb(wse);
        }
    } catch(const std::exception& e) {
        Loge(__func__, "Error: ", e.what());
        if (m_errorCb) {
            t_aif_event wse;
            wse.type = kWebSocketMessage;
            wse.data = e.what();
            m_errorCb(wse);
        }
    }
}

void WSClient::close(
    CloseCallback closeCb)
{
    // Save these for later
    if (closeCb != nullptr)
        m_closeCb = closeCb;

    // Close the WebSocket connection
    m_ws.async_close(websocket::close_code::normal,
        beast::bind_front_handler(
            &WSClient::onClose,
            shared_from_this()));
}

void WSClient::onClose(beast::error_code ec)
{
    try {
        if (ec) throw beast::system_error(ec);

        TRACE("connection closed");
        if (m_closeCb) {
            t_aif_event wse;
            wse.type = kWebSocketClose;
            wse.data = "connection closed";
            m_closeCb(wse);
        }

    } catch(beast::system_error const& se) {
        Loge(__func__, "Error: [", se.code() , "] ", se.code().message());
        if (m_errorCb) {
            t_aif_event wse;
            wse.type = kWebSocketClose;
            wse.data = se.code().message();
            m_errorCb(wse);
        }
    } catch(const std::exception& e) {
        Loge(__func__, "Error: ", e.what());
        if (m_errorCb) {
            t_aif_event wse;
            wse.type = kWebSocketClose;
            wse.data = e.what();
            m_errorCb(wse);
        }
    }
}

} // end of namespace aif
