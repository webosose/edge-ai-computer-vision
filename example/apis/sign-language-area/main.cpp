/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/facade/EdgeAIVision.h>
#include <aif/tools/Utils.h>
#include <aif/tools/PerformanceReporter.h>
#include <Renderer.h>
#include <rapidjson/document.h>


using namespace aif;
namespace rj = rapidjson;

int main(int argc, char *argv[]) {
    std::cout << "Usage: apis-sign-language-area-sample <input-file-path> <output-file-path> <config-path>" << std::endl;

    std::string config = "";
    std::string inputPath = R"(/usr/share/aif/images/signlanguagearea.png)";
    std::string outputPath  = R"(./res.jpg)";
    std::string configPath = R"(./sign_language_area_cpu.json)";
    if (argc >= 2) {
        inputPath = argv[1];
    }
    if (argc >= 3) {
        outputPath = argv[2];
    }
    if (argc >= 4) {
        configPath = argv[3];
        config = fileToStr(configPath);
    }

    std::cout << "input : " << inputPath << std::endl;
    std::cout << "output: " << outputPath << std::endl;
    std::cout << "configPath: " << configPath<< std::endl;

    cv::Mat input = cv::imread(inputPath, cv::IMREAD_COLOR);

    std::string output;

    EdgeAIVision::DetectorType type = EdgeAIVision::DetectorType::SIGNLANGUAGEAREA;
    EdgeAIVision& ai = EdgeAIVision::getInstance();

    ai.startup();
    ai.createDetector(type, config);

    ai.detect(type, input, output);

    ai.deleteDetector(type);
    ai.shutdown();
    PerformanceReporter::get().showReport();

    std::cout << output << std:: endl;
    rj::Document d;
    d.Parse(output.c_str());

    double left   = d["signLanguageAreas"][0].GetDouble();
    double top    = d["signLanguageAreas"][1].GetDouble();
    double right  = d["signLanguageAreas"][2].GetDouble();
    double bottom = d["signLanguageAreas"][3].GetDouble();

    constexpr int input_width  = 640;
    constexpr int input_height = 360;

    int left_x      = left * input_width;
    int top_y       = top * input_height;
    int rect_width  = (right - left) * input_width;
    int rect_height = (bottom - top) * input_height;

    std::vector<cv::Rect> rects;
    rects.push_back( {left_x, top_y, rect_width, rect_height} );

    cv::Mat result = Renderer::drawRects(input, rects, cv::Scalar(0, 0, 255), 3);

    cv::imwrite(outputPath, result);

    return 0;
}
