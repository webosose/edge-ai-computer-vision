/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/pipe/Pipe.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <string>

using namespace aif;

int main(int argc, char* argv[])
{
    std::string config = R"(
        {
            "name" : "pipe_face",
            "nodes": [
                {
                    "id" : "detect_face",
                    "input" : ["image"],
                    "output" : ["image", "inference"],
                    "operation" : {
                        "type" : "detector",
                        "config": {
                            "model": "face_yunet_cpu",
                            "param": {
                                "autoDelegate": {
                                    "policy": "CPU_ONLY"
                                }
                            }
                        }
                    }
                }
            ]
        })";

    std::string inputPath = R"(/usr/share/aif/images/person.jpg)";

    if (argc == 2) {
        std::cout << "Usage: pipe-sample <config-file-path> <input-file-path>" << std::endl;
        std::cout << "Example: pipe-sample face.json " << inputPath << std::endl;
        std::cout << "Example Config Json: " << std::endl << config << std::endl;
        return 0;
    }
    std::string configPath;
    if (argc == 3) {
        configPath = argv[1];
        inputPath = argv[2];
    }

    if (!configPath.empty()) {
        std::ifstream ifs(configPath);
        if (!ifs.is_open()) {
            std::cout << "failed to read file : " << configPath << std::endl;
            return 0;
        }
        config.clear();
        std::string line;
        while (getline(ifs, line)){
            config.append("\n");
            config.append(line);
        }
    }


    AIVision::init();
    Pipe pipe;
    if (!pipe.build(config)) {
        std::cout << "failed to build pipe" << std::endl;
    }

    cv::Mat image = cv::imread(inputPath);
    if (!pipe.detect(image)) {
        std::cout << "failed to build pipe" << std::endl;
    }

    std::cout << "Result: " << std::endl << pipe.getDescriptor()->getResult() << std::endl;
    AIVision::deinit();

    return 0;
}
