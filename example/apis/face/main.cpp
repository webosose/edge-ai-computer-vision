/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/facade/EdgeAIVision.h>

using namespace aif;

int main()
{
    cv::Mat input = cv::imread("/usr/share/aif/images/person.jpg", cv::IMREAD_COLOR);
    cv::Mat input2 = cv::imread("/usr/share/aif/images/5faces.jpg", cv::IMREAD_COLOR);
    std::string output;

    EdgeAIVision::DetectorType type = EdgeAIVision::DetectorType::FACE;
    EdgeAIVision ai = EdgeAIVision::getInstance();

    ai.startup();
    ai.createDetector(type);

    ai.detect(type, input, output);
    std::cout << output << std:: endl;

    ai.detect(type, input2, output);
    std::cout << output << std:: endl;


    ai.deleteDetector(type);
    ai.shutdown();

    return 0;
}
