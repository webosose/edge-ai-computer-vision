#include <aif/base/WSServer.h>
#include <aif/base/WSClient.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <functional>

using namespace aif;

#if 0
//--------------------------------------------------
// Echo server
//--------------------------------------------------
class EchoSession : public WSServerSession
{
public:
    EchoSession(tcp::socket&& socket)
        : WSServerSession(std::move(socket))
    {}

    ~EchoSession() override
    {
    }

    void onInit() override
    {
        TRACE("onInit()");
    }

    void onHandleRequest(const std::string& request) override
    {
        TRACE("onHandleRequest(): ", request);
    }
};

class EchoServer : public WSServer
{
public:
    EchoServer(asio::io_context& ioc, tcp::endpoint endpoint)
        : WSServer(ioc, endpoint)
    {}

    ~EchoServer() override
    {}

    std::shared_ptr<WSServerSession> createSession(tcp::socket&& socket) override
    {
        return std::make_shared<EchoSession>(std::move(socket));
    }
};

//--------------------------------------------------
// Asynchronous Echo client
//--------------------------------------------------

using EchoCallback = std::function<void(const std::string&)>;

class EchoClient : public WSClient
{
    EchoCallback m_echoCb;
public:
    EchoClient(asio::io_context& ioc)
        : WSClient(ioc)
        , m_echoCb(nullptr)
    {}

    ~EchoClient() override
    {}

    t_aif_status echo(std::string& message, EchoCallback cb = nullptr)
    {
        try {

            if (cb) {
                m_echoCb = cb;
            }

            // Send the message
            m_ws.async_write(
                asio::buffer(message),
                beast::bind_front_handler(
                    &EchoClient::onSendEcho,
                    std::static_pointer_cast<EchoClient>(shared_from_this())));

            return kAifOk;
        } catch(beast::system_error const& se) {
            if(se.code() == websocket::error::closed) {
                Logi("connection closed.");
            } else {
                Loge(__func__, "Error: [", se.code() , "] ", se.code().message());
            }
            return kAifError;
        } catch(const std::exception& e) {
            Loge(__func__, "Error: ", e.what());
            return kAifError;
        }
    }

private:
    void onSendEcho(
        beast::error_code ec,
        std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        try {
            if (ec)
                throw beast::system_error(ec);

            // Read a message into our buffer
            m_ws.async_read(
                m_buffer,
                beast::bind_front_handler(
                    &EchoClient::onRecvEcho,
                    std::static_pointer_cast<EchoClient>(shared_from_this())));

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

    void onRecvEcho(
        beast::error_code ec,
        std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        try {
            if (ec)
                throw beast::system_error(ec);

            // read echo message....

            if (m_echoCb != nullptr) {
                m_echoCb("Received echo message..");
            }

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
};

#endif

//--------------------------------------------------
// WebSocketServer Test
//--------------------------------------------------
class WebSocketTest : public ::testing::Test
{
protected:
    WebSocketTest() = default;
    ~WebSocketTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(WebSocketTest, ws01_wsserver)
{
    try {
        auto const address = asio::ip::make_address("0.0.0.0");
        auto const port = static_cast<unsigned short>(std::atoi("8080"));
        auto const threads = 1;

        // The io_context is required for all I/O
        asio::io_context ioc{threads};

        // Create and launch a listening port
        std::make_shared<WSServer>(ioc, tcp::endpoint{address, port})->run();

        // Run the I/O service on the requested number of threads
        std::vector<std::thread> v;
        v.reserve(threads - 1);
        for(auto i = threads - 1; i > 0; --i)
            v.emplace_back(
            [&ioc]
            {
                ioc.run();
            });
        ioc.run();

        EXPECT_TRUE(true);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        EXPECT_TRUE(false);
    }
}

TEST_F(WebSocketTest, ws02_wsclient_async)
{
    auto const host = "0.0.0.0";
    auto const port = "8080";

    // The io_context is required for all I/O
    asio::io_context ioc;
    auto client = std::make_shared<WSClient>(ioc, [&](const t_aif_event& e) {
        TRACE("e.type: ", e.type, ", e.data: ", e.data);
    });

    client->open(host, port, [&](const t_aif_event& e) {
        TRACE("e.type: ", e.type, ", e.data: ", e.data);
        EXPECT_TRUE(e.type == kWebSocketOpen);

        client->sendMessage("hello", [&](const t_aif_event& e) {
                TRACE("e.type: ", e.type, ", e.data: ", e.data);
                EXPECT_TRUE(e.type == kWebSocketMessage);
                EXPECT_TRUE(e.data == "hello");

                client->close([](const t_aif_event& e) {
                     TRACE("e.type: ", e.type, ", e.data: ", e.data);
                     EXPECT_TRUE(e.type == kWebSocketClose);
                });
            });
    });

    // Run the I/O service. The call will return when
    // the socket is closed.
    ioc.run();
}
