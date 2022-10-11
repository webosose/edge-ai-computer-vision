/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/facade/EdgeAIVision.h>
#include <fstream>

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

    cv::Mat image = cv::imread(inputPath);

    std::string id = "pipe_face";
    EdgeAIVision& ai = EdgeAIVision::getInstance();
    if (!ai.startup()) {
        std::cout << "failed to start edgeai vision" << std::endl;
    }

    if (!ai.pipeCreate(id, config)) {
        std::cout << "failed to create pipe: " << config << std::endl;
    }

    std::string output;
    if (!ai.pipeDetect(id, image, output)) {
        std::cout << "failed to build pipe" << std::endl;
    }
    std::cout << "Result: " << std::endl << output << std::endl;

    if (!ai.pipeDelete(id)) {
        std::cout << "failed to delete pipe: " << id << std::endl;
    }

    if (!ai.shutdown()) {
        std::cout << "failed to shutdown edgeai vision" << std::endl;
    }

    return 0;
}
