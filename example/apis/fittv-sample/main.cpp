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
            "name" : "fittv_pipe",
            "descriptor": "fittv_pose",
            "nodes": [
                {
                    "id" : "detect_person",
                    "input" : ["image"],
                    "output" : ["image", "inference"],
                    "operation" : {
                        "type" : "detector",
                        "config": {
                            "model": "person_yolov3_v1_npu",
                            "param": {
                                "delegates": [
                                    {
                                        "name": "npu_delegate",
                                        "option": {}
                                    }
                                ]
                            }
                        }
                    }
                },
                {
                    "id" : "person_crop",
                    "input" : ["image", "inference"],
                    "output" : ["image", "inference"],
                    "operation" : {
                        "type" : "fittv_person_crop",
                        "config": {
                            "targetId" : "detect_person"
                        }
                    }
                },
                {
                    "id" : "detect_pose2d",
                    "input" : ["image", "inference"],
                    "output" : ["image", "inference"],
                    "operation" : {
                        "type" : "fittv_pose2d_detector",
                        "config": {
                            "model": "pose2d_resnet_cpu",
                            "param": {
                                "autoDelegate": {
                                    "policy": "MIN_LATENCY"
                                }
                            }
                        }
                    }
                },
                {
                    "id" : "2d3d_bridge",
                    "input" : ["image", "inference"],
                    "output" : ["image", "inference"],
                    "operation" : {
                        "type" : "fittv_pose2d3d",
                        "config": {
                            "targetId" : "detect_pose2d"
                        }
                    }
                },
                {
                    "id" : "detect_pose3d",
                    "input" : ["image", "inference"],
                    "output" : ["image", "inference"],
                    "operation" : {
                        "type" : "fittv_pose3d_detector",
                        "config": {
                            "model": "pose3d_videopose3d_npu",
                            "param": {
                                "delegates": [
                                    {
                                        "name": "npu_delegate",
                                        "option": {}
                                    }
                                ]
                            }
                        }
                    }
                }

            ]
        })";

    std::string inputPath = R"(/usr/share/aif/images/person.jpg)";
    std::string outputPath  = R"(./res.jpg)";

    if (argc == 2) {
        std::cout << "Usage: fittv-sample <config-file-path> <input-file-path>" << std::endl;
        std::cout << "Example: fittv-sample face.json " << inputPath << std::endl;
        std::cout << "Example Config Json: " << std::endl << config << std::endl;
        return 0;
    }
    std::string configPath;
    if (argc >= 3) {
        configPath = argv[1];
        inputPath = argv[2];
    }
    if (argc >= 4) {
        outputPath = argv[3];
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

    std::string id = "fittv_pipe";
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
    std::cout << "Input: " << inputPath << std::endl;
    std::cout << "Output: " << std::endl << output << std::endl;

    if (!ai.pipeDelete(id)) {
        std::cout << "failed to delete pipe: " << id << std::endl;
    }

    if (!ai.shutdown()) {
        std::cout << "failed to shutdown edgeai vision" << std::endl;
    }

    return 0;
}
