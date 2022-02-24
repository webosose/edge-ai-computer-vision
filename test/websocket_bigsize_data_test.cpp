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
#include <rapidjson/istreamwrapper.h>

using namespace aif;
namespace rj = rapidjson;

//--------------------------------------------------
// JSON BigSizeData server
//--------------------------------------------------
class BigSizeDataSession : public WSServerSession
{
public:
    BigSizeDataSession(tcp::socket&& socket)
        : WSServerSession(std::move(socket))
    {}

    ~BigSizeDataSession() override
    {
    }

    void onHandleMessage(const std::string& message) override
    {
        // json parsing
        rj::Document payload;
        payload.Parse(message.c_str());

        std::string req = payload["req"].GetString();

        if (req == "BigSizeData") {
            onHandleBigSizeData();
        } else {
            onHandleError(req);
        }
    }

    void onHandleBigSizeData()
    {
        std::ifstream ifs("/usr/share/aif/model/data.json");
        rj::IStreamWrapper isw(ifs);
        rj::Document res;
        res.ParseStream(isw);

        rj::StringBuffer buffer;
        rj::Writer<rj::StringBuffer> writer(buffer);
        res.Accept(writer);

        response(buffer.GetString());
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

class BigSizeDataServer : public WSServer
{
public:
    BigSizeDataServer(asio::io_context& ioc, tcp::endpoint endpoint)
        : WSServer(ioc, endpoint)
    {}

    ~BigSizeDataServer() override
    {}

    std::shared_ptr<WSServerSession> createSession(tcp::socket&& socket) override
    {
        return std::make_shared<BigSizeDataSession>(std::move(socket));
    }
};

//--------------------------------------------------
// JSON BigSizeData client
//--------------------------------------------------

using BigSizeDataCallback = std::function<void(const std::string&)>;

class BigSizeDataClient : public WSClient
{
public:
    BigSizeDataClient(asio::io_context& ioc, ErrorCallback errorCb = nullptr)
        : WSClient(ioc, errorCb)
    {}

    ~BigSizeDataClient() override
    {}

    t_aif_status getBigSizeDataAsync(BigSizeDataCallback cb)
    {
        // { req: "BigSizeData" }
        rj::Document payload;
        payload.SetObject();
        payload.AddMember("req", "BigSizeData", payload.GetAllocator());

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
class BigSizeDataWebSocketTest : public ::testing::Test
{
protected:
    BigSizeDataWebSocketTest() = default;
    ~BigSizeDataWebSocketTest() = default;

    void SetUp() override
    {
        Logger::init(LogLevel::TRACE4);
    }

    void TearDown() override
    {
    }
};

TEST_F(BigSizeDataWebSocketTest, bsd01_BigSizeData_server)
{
    try {
        auto const address = asio::ip::make_address("0.0.0.0");
        auto const port = static_cast<unsigned short>(std::atoi("8080"));
        auto const threads = 4;

        // The io_context is required for all I/O
        asio::io_context ioc{threads};

        // Create and launch a listening port
        std::make_shared<BigSizeDataServer>(ioc, tcp::endpoint{address, port})->run();

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

TEST_F(BigSizeDataWebSocketTest, bsd02_BigSizeData_client)
{
    auto const host = "0.0.0.0";
    auto const port = "8080";

    // The io_context is required for all I/O
    asio::io_context ioc;
    auto client = std::make_shared<BigSizeDataClient>(ioc, [&](const t_aif_event& e) {
        TRACE("e.type: ", e.type, ", e.data: ", e.data);
    });

    client->open(host, port, [&](const t_aif_event& e) {
        TRACE("e.type: ", e.type, ", e.data: ", e.data);
        EXPECT_TRUE(e.type == kWebSocketOpen);

        client->getBigSizeDataAsync([&](const std::string& data){
            // TRACE("BigSizeData: ", data);
            EXPECT_TRUE(data.size() > 0);
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
