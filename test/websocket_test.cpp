#include <aif/base/WSServer.h>
#include <aif/base/WSClient.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <functional>

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

using namespace aif;
namespace rj = rapidjson;

//--------------------------------------------------
// JSON DayTime server
//--------------------------------------------------
class DayTimeSession : public WSServerSession
{
public:
    DayTimeSession(tcp::socket&& socket)
        : WSServerSession(std::move(socket))
    {}

    ~DayTimeSession() override
    {
    }

    void onHandleMessage(const std::string& message) override
    {
        // json parsing
        rj::Document payload;
        payload.Parse(message.c_str());

        std::string req = payload["req"].GetString();

        if (req == "daytime") {
            onHandleDayTime();
        } else {
            onHandleError(req);
        }
    }

    void onHandleDayTime()
    {
        // { returnCode: 0, daytime: "xxx" }
        rj::Document res;
        res.SetObject();
        res.AddMember("returnCode", 0, res.GetAllocator());
        res.AddMember("daytime", daytimeString(), res.GetAllocator());

        rj::StringBuffer buffer;
        rj::Writer<rj::StringBuffer> writer(buffer);
        res.Accept(writer);

        response(buffer.GetString());
    }

    std::string daytimeString()
    {
        std::stringstream msg;
        std::time_t now = time(nullptr);
        msg << std::ctime(&now);
        return msg.str();
    }

    void onHandleError(const std::string& req)
    {
        // { returnCode: 1, error: "xxx" }
        rj::Document res;
        res.SetObject();
        res.AddMember("returnCode", 1, res.GetAllocator());
        res.AddMember("error", std::string("unknow request: ") + req, res.GetAllocator());

        rj::StringBuffer buffer;
        rj::Writer<rj::StringBuffer> writer(buffer);
        res.Accept(writer);

        response(buffer.GetString());
    }
};

class DayTimeServer : public WSServer
{
public:
    DayTimeServer(asio::io_context& ioc, tcp::endpoint endpoint)
        : WSServer(ioc, endpoint)
    {}

    ~DayTimeServer() override
    {}

    std::shared_ptr<WSServerSession> createSession(tcp::socket&& socket) override
    {
        return std::make_shared<DayTimeSession>(std::move(socket));
    }
};

//--------------------------------------------------
// JSON DayTime client
//--------------------------------------------------

using DayTimeCallback = std::function<void(const std::string&)>;

class DayTimeClient : public WSClient
{
public:
    DayTimeClient(asio::io_context& ioc, ErrorCallback errorCb = nullptr)
        : WSClient(ioc, errorCb)
    {}

    ~DayTimeClient() override
    {}

    t_aif_status getDayTimeAsync(DayTimeCallback cb)
    {
        // { req: "daytime" }
        rj::Document payload;
        payload.SetObject();
        payload.AddMember("req", "daytime", payload.GetAllocator());

        rj::StringBuffer buffer;
        rj::Writer<rj::StringBuffer> writer(buffer);
        payload.Accept(writer);

        sendMessage(buffer.GetString(), [&](const t_aif_event& e){
            TRACE("e.type: ", e.type, ", e.data: ", e.data);
            if (cb) {
                cb(e.data);
            }
        });

        return kAifOk;
    }
};

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
        Logger::init(LogLevel::TRACE4);
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

                client->close([&](const t_aif_event& e) {
                     TRACE("e.type: ", e.type, ", e.data: ", e.data);
                     EXPECT_TRUE(e.type == kWebSocketClose);
                });
            });
    });

    // Run the I/O service. The call will return when
    // the socket is closed.
    ioc.run();
}

TEST_F(WebSocketTest, ws03_daytime_server)
{
    try {
        auto const address = asio::ip::make_address("0.0.0.0");
        auto const port = static_cast<unsigned short>(std::atoi("8080"));
        auto const threads = 1;

        // The io_context is required for all I/O
        asio::io_context ioc{threads};

        // Create and launch a listening port
        std::make_shared<DayTimeServer>(ioc, tcp::endpoint{address, port})->run();

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

TEST_F(WebSocketTest, ws04_daytime_client)
{
    auto const host = "0.0.0.0";
    auto const port = "8080";

    // The io_context is required for all I/O
    asio::io_context ioc;
    auto client = std::make_shared<DayTimeClient>(ioc, [&](const t_aif_event& e) {
        TRACE("e.type: ", e.type, ", e.data: ", e.data);
    });

    client->open(host, port, [&](const t_aif_event& e) {
        TRACE("e.type: ", e.type, ", e.data: ", e.data);
        EXPECT_TRUE(e.type == kWebSocketOpen);

        client->getDayTimeAsync([&](const std::string& data){
            TRACE("daytime: ", data);
            EXPECT_TRUE(data.size() > 0);
            // client->close([&](const t_aif_event& e) {
            //     TRACE("e.type: ", e.type, ", e.data: ", e.data);
            //     EXPECT_TRUE(e.type == kWebSocketClose);
            // });
        });
    });

    // Run the I/O service. The call will return when
    // the socket is closed.
    ioc.run();
}
