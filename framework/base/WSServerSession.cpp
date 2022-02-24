#include <aif/base/WSServerSession.h>
#include <aif/log/Logger.h>

#include <boost/beast/websocket.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/array.hpp>

#include <msgpack.hpp>

// #define RAPIDJSON_HAS_STDSTRING 1
// #include <rapidjson/document.h>
// #include <rapidjson/writer.h>
// #include <rapidjson/stringbuffer.h>

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <stdexcept>
#include <sstream>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp
namespace net = boost::asio;            // from <boost/asio.hpp>
// namespace rj = rapidjson;

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
// using aif::Stopwatch;

namespace {

} // end of anonymous namespace

namespace aif {

WSServerSession::WSServerSession(tcp::socket&& socket)
    : m_ws{std::move(socket)}
    , m_outBuffer(std::make_shared<msgpack::sbuffer>())
{
}

WSServerSession::~WSServerSession()
{
}

// Get on the correct executor
void WSServerSession::run()
{
    // We need to be executing within a strand to perform async operations
    // on the I/O objects in this session. Although not strictly necessary
    // for single-threaded contexts, this example code is written to be
    // thread-safe by default.
    net::dispatch(m_ws.get_executor(),
        beast::bind_front_handler(
            &WSServerSession::onRun,
            shared_from_this()));
}

/*virtual */
void WSServerSession::onInit()
{
}

// Start the asynchronous operation
void WSServerSession::onRun()
{
    try {
        onInit();

        // Set suggested timeout settings for the websocket
        m_ws.set_option(
            websocket::stream_base::timeout::suggested(
                beast::role_type::server));

        // Set a decorator to change the Server of the handshake
        m_ws.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res)
            {
                res.set(http::field::server,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                        " aif-websocket-server");
            }));

        // Accept the websocket handshake
        m_ws.async_accept(
            beast::bind_front_handler(
                &WSServerSession::onAccept,
                shared_from_this()));


    } catch(beast::system_error const& se) {
        if(se.code() == websocket::error::closed) {
            Logi(__func__, "[", se.code() , "] ", se.code().message());
        } else {
            Loge(__func__, "Error: [", se.code() , "] ", se.code().message());
        }
    } catch(const std::exception& e) {
        Loge(__func__, "Error: ", e.what());
    }
}

void WSServerSession::onAccept(beast::error_code ec)
{
    try {
        if (ec) throw beast::system_error(ec);

        // Read from a client message
        readMessage();

    } catch(beast::system_error const& se) {
        if(se.code() == websocket::error::closed) {
            Logi(__func__, "[", se.code() , "] ", se.code().message());
        } else {
            Loge(__func__, "Error: [", se.code() , "] ", se.code().message());
        }
    } catch(const std::exception& e) {
        Loge(__func__, "Error: ", e.what());
    }
}

void WSServerSession::readMessage()
{
    // Read a message into our buffer
    m_ws.binary(true);
    m_ws.async_read(
        m_buffer,
        beast::bind_front_handler(
            &WSServerSession::onMessage,
            shared_from_this()));
}

void WSServerSession::onMessage(
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

        onHandleMessage(message);

    } catch(beast::system_error const& se) {
        if(se.code() == websocket::error::closed) {
            Logi(__func__, "[", se.code() , "] ", se.code().message());
        } else {
            Loge(__func__, "Error: [", se.code() , "] ", se.code().message());
        }
    } catch(const std::exception& e) {
        Loge(__func__, "Error: ", e.what());
    }
}

/*virtual */
void WSServerSession::onHandleMessage(const std::string& message)
{
    // default action is echo
    // a subclass has to override this method
    response(message);
}

void WSServerSession::response(const std::string& message)
{
    // encode message
    m_outBuffer->clear();
    msgpack::packer<msgpack::sbuffer> packer(*m_outBuffer);
    packer.pack(message);

    // Send the response
    m_ws.binary(true);
    m_ws.async_write(
        asio::buffer(m_outBuffer->data(), m_outBuffer->size()),
        beast::bind_front_handler(
            &WSServerSession::onResponse,
            shared_from_this()));
}

void WSServerSession::onResponse(
    beast::error_code ec,
    std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    try {
        if (ec) throw beast::system_error(ec);

        // Clear the buffer
        m_buffer.consume(m_buffer.size());

        // Do another read
        readMessage();

    } catch(beast::system_error const& se) {
        if(se.code() == websocket::error::closed) {
            Logi(__func__, "[", se.code() , "] ", se.code().message());
        } else {
            Loge(__func__, "Error: [", se.code() , "] ", se.code().message());
        }
    } catch(const std::exception& e) {
        Loge(__func__, "Error: ", e.what());
    }
}

// Report a failure
void WSServerSession::fail(beast::error_code ec, char const* what)
{
    AIF_ERROR << what << ": " << ec.message();
}

} // end of namespace aif
