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
#include <cctype>

#include <aif/tools/Renderer.h>
#include <aif/bodyPoseEstimation/fittv/FitTvPoseDescriptor.h>

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
                            "model": "person_yolov3_npu",
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
        std::cout << "Usage: fittv-sample <config-file-path> <input-file-path> [opt: <output-file-path> <num_iterator>]" << std::endl;
        std::cout << "Example: fittv-sample face.json " << inputPath << std::endl;
        std::cout << "Example: fittv-sample face.json " << inputPath << " 100 " << std::endl;
        std::cout << "Example: fittv-sample face.json " << inputPath << " " << outputPath << " 100 " << std::endl;
        std::cout << "Example Config Json: " << std::endl << config << std::endl;
        return 0;
    }
    std::string configPath;
    if (argc >= 3) {
        configPath = argv[1];
        inputPath = argv[2];
    }
    int num_iterator = 1;
    if (argc >= 4) {
        std::string argv3(argv[3]);
        bool isNumber = true;
        for (int i = 0; i < argv3.length(); i++) {
            if (!isdigit(argv3[i])) {
                isNumber = false;
                break;
            }
        }

        if (isNumber) {
            num_iterator = atoi(argv[3]);
        } else {
            outputPath = argv[3];
        }
    }
    if (argc >= 5) {
        num_iterator = atoi(argv[4]);
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
    for (int i = 0; i < num_iterator; i++) {     // num_iterator = 1 in default
        if (!pipe.detect(image)) {
            std::cout << "failed to build pipe" << std::endl;
        }
    }

    std::cout << "Input: " << inputPath << std::endl;
    std::cout << "Output: " << std::endl << pipe.getDescriptor()->getResult() << std::endl;
    auto fd = std::dynamic_pointer_cast<FitTvPoseDescriptor>(pipe.getDescriptor());
    cv::Mat result = fd->getImage();
    for (auto& keyPoints : fd->getKeyPoints()) {
        result = Renderer::drawPose2d(result, keyPoints);
    }

    result = Renderer::drawRects(result, fd->getCropRects(), cv::Scalar(255, 0, 0), 1);
    result = Renderer::drawBoxes(result, fd->getBboxes(), cv::Scalar(0, 0, 255), 2);

    cv::imwrite(outputPath, result);
    AIVision::deinit();

    return 0;
}
