/*
 * Copyright (c) 2022-2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/facade/EdgeAIVision.h>
#include <aif/tools/PerformanceReporter.h>
#include <nlohmann/json.hpp>

using namespace aif;
using json = nlohmann::json;

int main()
{
    cv::Mat input = cv::imread("/usr/share/aif/images/person.jpg", cv::IMREAD_COLOR);
    cv::Mat input2 = cv::imread("/usr/share/aif/images/5faces.jpg", cv::IMREAD_COLOR);
    std::string output;

    EdgeAIVision::DetectorType type = EdgeAIVision::DetectorType::FACE;
    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ai.startup();

    std::cout << "EdgeAIVision version isInitialized" << std::endl;
    std::getchar();

    std::string facetype = "";
    auto solutions = ai.getCapableSolutionList();
    for (auto &sol : solutions) {
        if (sol.find("face_yunet") != std::string::npos) {
            facetype = sol;
        }
    }
    std::cout << "facetype: " << facetype << std::endl;

    auto defaultConfig = ai.generateAIFParam(facetype);

    std::cout << defaultConfig << std::endl;

    auto param = json::parse(defaultConfig, nullptr, false);

    ai.createDetector(type, defaultConfig);

    std::cout << "Detector created" << std::endl;
    std::getchar();

    for (int i = 0; i < 10; i++) {
        ai.detect(type, input, output);
        std::cout << output << std:: endl;

        ai.detect(type, input2, output);
        std::cout << output << std:: endl;
    }

    ai.deleteDetector(type);
    ai.shutdown();
    std::cout << "EdgeAIVision shutdowned" << std::endl;
    std::getchar();

    PerformanceReporter::get().showReport();
    return 0;
}
