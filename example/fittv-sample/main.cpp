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
                            "model": "person_yolov4_npu",
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
                            "model": "pose2d_resnet_npu",
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

    if (argc == 2) {
        std::cout << "Usage: fittv-pipe-sample <config-file-path> <input-file-path>" << std::endl;
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
    auto fd = std::dynamic_pointer_cast<FitTvPoseDescriptor>(pipe.getDescriptor());

    cv::Mat result = fd->getImage();
    for (auto& keyPoints : fd->getKeyPoints()) {
        result = Renderer::drawPose2d(result, keyPoints);
    }
    cv::imwrite("./res.jpg", result);
    AIVision::deinit();

    return 0;
}
