/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/facade/EdgeAIVision.h>
#include <aif/tools/Utils.h>
#include <rapidjson/document.h>


using namespace aif;
namespace rj = rapidjson;

cv::Mat drawRects(
        const cv::Mat &img,
        const std::vector<cv::Rect>& rects,
        cv::Scalar color,
        int thickness)
{
    cv::Mat temp = img;
    for (const auto& rect : rects) {
        cv::rectangle(img, rect, color, thickness);
    }
    return temp;
}

int main(int argc, char *argv[]) {
    std::cout << "Usage: apis-text-sample <input-file-path> <output-file-path> <config-path>" << std::endl;

    std::string config = "";
    std::string inputPath = R"(/usr/share/aif/images/text.jpg)";
    std::string outputPath  = R"(./res.jpg)";
    std::string configPath = R"(./text.json)";
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

    EdgeAIVision::DetectorType type = EdgeAIVision::DetectorType::TEXT;
    EdgeAIVision& ai = EdgeAIVision::getInstance();

    ai.startup();
    ai.createDetector(type, config);

    ai.detect(type, input, output);

    ai.deleteDetector(type);
    ai.shutdown();

    std::cout << output << std:: endl;
    rj::Document d;
    d.Parse(output.c_str());

    std::vector<cv::Rect> rects;
    for (int i = 0; i < d["texts"].Size(); i++) {
        rects.push_back({
            d["texts"][i][0].GetInt(),
            d["texts"][i][1].GetInt(),
            d["texts"][i][2].GetInt(),
            d["texts"][i][3].GetInt()
        });
    }
    cv::Mat result = drawRects(input, rects, cv::Scalar(0, 0, 255), 3);
    cv::imwrite(outputPath, result);

    return 0;
}
