/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/facade/EdgeAIVision.h>
#include <aif/tools/Utils.h>
#include <rapidjson/document.h>

#include <vector>
#include <memory>
#include <thread>

using namespace aif;
namespace rj = rapidjson;

class TestThread {
    public:
        TestThread( const std::string& name, EdgeAIVision* ai, const cv::Mat input, int count)
            : m_name(name), m_ai(ai), m_input(input), m_count(count) {}
        ~TestThread() {}

        TestThread(TestThread&& other) noexcept
            : m_name{std::move(other.m_name)}
        , m_ai{std::move(other.m_ai)}
        , m_input{std::move(other.m_input)}
        , m_count{std::move(other.m_count)} {}

        TestThread& operator=(TestThread&& other) noexcept {
            if (this == &other) {
                return *this;
            }

            m_name = other.m_name;
            m_ai = other.m_ai;
            m_input = other.m_input;
            m_count = other.m_count;

            return *this;
        }


        void operator()() {
            EdgeAIVision::DetectorType type = EdgeAIVision::DetectorType::TEXT;
            std::string output;
            for (int i = 0; i < m_count; i++) {
                m_ai->detect(type, m_input, output);
                std::cout << m_name << " : " << i << std::endl;
            }
        }

    private:
        std::string m_name;
        EdgeAIVision* m_ai;
        int m_count;
        cv::Mat m_input;
};

void showHelp() {
    std::cout << "quit : quit test" << std::endl;
    std::cout << "# step test #########################################" << std::endl;
    std::cout << "startup : startup edgeai-vision" << std::endl;
    std::cout << "create : create detector" << std::endl;
    std::cout << "detect <num> : detect * <num>" << std::endl;
    std::cout << "delete : delete detector" << std::endl;
    std::cout << "shutdown : shutdown edgeai-vision" << std::endl << std::endl;
    std::cout << "# execution test #########################################" << std::endl;
    std::cout << "test_detect <num> (usage: test_detect 10)" << std::endl;
    std::cout << "  1. startup" << std::endl;
    std::cout << "  2. createDetector" << std::endl;
    std::cout << "  3. dectect * <num>" << std::endl;
    std::cout << "  4. deleteDetector" << std::endl;
    std::cout << "  5. shutdown" << std::endl;
    std::cout << "test_thread <num> <thread num> (usage: test_thread 10 4)" << std::endl;
    std::cout << "  1. startup" << std::endl;
    std::cout << "  2. createDetector" << std::endl;
    std::cout << "  3. create <thread num> threads" << std::endl;
    std::cout << "  4. each thread detect * <num>" << std::endl;
    std::cout << "  5. deleteDetector" << std::endl;
    std::cout << "  6. shutdown" << std::endl;
    std::cout << "test_cd <num> <detection on/off (1/0)> (usage: test_cd 100 1)" << std::endl;
    std::cout << "  1. startup" << std::endl;
    std::cout << "  2. (createDetector - detect - deleteDetector) * <num>" << std::endl;
    std::cout << "  3. shutdown" << std::endl;
 }

int main(int argc, char *argv[]) {
    std::cout << "Usage: memory-sample <config-file-path>" << std::endl;

    std::string config = "";
    std::string inputPath = R"(/usr/share/aif/images/text.jpg)";
    std::string configPath = R"(./text.json)";
    if (argc >= 2) {
        configPath = argv[1];
        config = fileToStr(configPath);
    }

    std::cout << "input : " << inputPath << std::endl;
    std::cout << "configPath: " << configPath<< std::endl;

    cv::Mat input = cv::imread(inputPath, cv::IMREAD_COLOR);
    std::string output;
    EdgeAIVision::DetectorType type = EdgeAIVision::DetectorType::TEXT;
    EdgeAIVision& ai = EdgeAIVision::getInstance();

    while (1) {
        std::cout << "command (help - show guide): ";
        std::string cmd;
        std::cin >> cmd;
        if (cmd == "quit") {
            break;
        } else if (cmd == "startup") {
            if (ai.startup()) {
                std::cout << "started" << std::endl;
            } else {
                std::cout << "failed to startup" << std::endl;
            }
        } else if (cmd == "help") {
            showHelp();
        } else if (cmd == "test_cd") {
            int num = 0;
            int detectOnOff = 0;
            std::cin >> num >> detectOnOff;
            ai.startup();
            for (int i = 0; i < num; i++) {
                ai.createDetector(type, config);
                if (detectOnOff)
                    ai.detect(type, input, output);
                ai.deleteDetector(type);
            }
            ai.shutdown();
        }  else if (cmd  == "test_detect") {
            ai.startup();
            ai.createDetector(type, config);
            int num = 0;
            std::cin >> num;
            for (int i = 0; i < num; i++) {
                ai.detect(type, input, output);
            }
            ai.detect(type, input, output);
            ai.shutdown();
        } else if (cmd == "test_thread") {
            ai.startup();
            ai.createDetector(type, config);
            int num = 0;
            int threadNum = 0;
            std::cin >> num >> threadNum;
            std::vector<std::unique_ptr<std::thread>> threads;
            for (int i = 0; i < threadNum; i++) {
                threads.push_back(std::make_unique<std::thread>(TestThread("Th" + std::to_string(i), &ai, input, num)));
            }
            for (int i = 0; i < threadNum; i++) {
                threads[i]->join();
            }
            ai.detect(type, input, output);
            ai.shutdown();
        } else {
            if (!ai.isStarted()) {
                std::cout << "First, use startup command" << std::endl;
                continue;
            }
            if (cmd == "create") {
                if (ai.createDetector(type, config)) {
                    std::cout << "created detector " << std::endl;
                } else {
                    std::cout << "failed to create detector" << std::endl;
                }
            } else if (cmd == "detect") {
                int num = 0;
                std::cin >> num;
                for (int i = 0; i < num; i++) {
                    ai.detect(type, input, output);
                }
            } else if (cmd == "delete") {
                if (ai.deleteDetector(type)) {
                    std::cout << "deleted detector " << std::endl;
                } else {
                    std::cout << "failed to delete detector" << std::endl;
                }
            }else if (cmd == "shutdown") {
                if (ai.shutdown()) {
                    std::cout << "shutdown ok" << std::endl;
                } else {
                    std::cout << "failed to shutdown" << std::endl;
                }
            } else {
                showHelp();
            }
        }
    }

    return 0;

}
