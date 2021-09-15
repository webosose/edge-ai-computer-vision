#include "FaceWSServer.h"

#include <aif/log/Logger.h>
#include <aif/tools/ConfigReader.h>

#include <iostream>

using namespace aif;

namespace {
} // anonymous namespace

int main(int argc, char* argv[])
{
    //ConfigReader reader("aif.config");
    //Logger::init(Logger::strToLogLevel(reader.getOption("LogLevel")));
    Logger::init(LogLevel::TRACE4);
    Logd("**", argv[0], "**");
    // Check command line arguments.
    if (argc != 4)
    {
        std::cerr <<
            "Usage: " << argv[0] << " <address> <port> <threads>\n" <<
            "Example:\n" <<
            "    " << argv[0] << " 0.0.0.0 8080 1\n";
        return EXIT_FAILURE;
    }
    auto const address = asio::ip::make_address(argv[1]);
    auto const port = static_cast<unsigned short>(std::atoi(argv[2]));
    auto const threads = std::max<int>(1, std::atoi(argv[3]));

    // The io_context is required for all I/O
    asio::io_context ioc{threads};

    // Create and launch a listening port
    std::make_shared<FaceWSServer>(ioc, tcp::endpoint{address, port})->run();

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

    return EXIT_SUCCESS;
}
