/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/Descriptor.h>
#include <aif/base/AIVision.h>
#include <aif/pipe/Pipe.h>
#include <aif/tools/ConfigReader.h>

#include <opencv2/opencv.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace rj = rapidjson;
using namespace aif;

int main(int argc, char* argv[])
{
    std::string config = R"(
        {
            "name": "pipe_face_mesh",
            "nodes": [
                {
                    "id": "detect_face",
                    "input": ["image"],
                    "output": ["image", "inference"],
                    "operation": {
                        "type": "detector",
                        "config": {
                            "model": "person_yolov3_v1_npu",
                            "param": {
                                "modelParam": {"detectObject": "FACE"}
                            }
                        }
                    }
                },
                {
                    "id": "face_crop",
                    "input": ["image","inference"],
                    "output": ["image","inference"],
                    "operation": {
                        "type": "face_crop",
                        "config": {"targetId": "detect_face"}
                    }
                },
                {
                    "id": "face_mesh",
                    "input": ["image","inference"],
                    "output": ["image","inference"],
                    "operation": {
                        "type": "detector",
                        "config": {
                            "model": "facemesh_cpu",
                            "param": {
                            "autoDelegate": {"policy": "CPU_ONLY"}
                            }
                        }
                    }
                },
                {
                    "id": "face_mesh_extract",
                    "input": ["image","inference"],
                    "output": ["image"],
                    "operation": {
                        "type": "face_mesh_extract",
                        "config": {"targetId": "face_mesh"}
                    }
                }
            ]
        }
    )";

    std::string inputPath = R"(/usr/share/aif/images/person.jpg)";

    if (argc == 2)
    {
        std::cout << "Usage: facemesh-sample <config-file-path> <input-file-path>" << std::endl;
        std::cout << "Example: facemesh-sample pipe_face_mesh.json " << inputPath << std::endl;
        std::cout << "Example Config Json: " << std::endl
                    << config << std::endl;
        return 0;
    }
    std::string configPath;
    if (argc == 3)
    {
        configPath = argv[1];
        inputPath = argv[2];
    }

    if (!configPath.empty())
    {
        std::ifstream ifs(configPath);
        if (!ifs.is_open())
        {
            std::cout << "failed to read file : " << configPath << std::endl;
            return 0;
        }
        config.clear();
        std::string line;
        while (getline(ifs, line))
        {
            config.append("\n");
            config.append(line);
        }
    }

    aif::AIVision::init();
    aif::Pipe pipe;
    if (!pipe.build(config))
    {
        std::cout << "failed to build pipe" << std::endl;
    }

    cv::Mat image = cv::imread(inputPath);
    if (!pipe.detect(image))
    {
        std::cout << "failed to build pipe" << std::endl;
    }

    std::cout << "Result: " << std::endl
              << pipe.getDescriptor()->getResult("face_mesh") << std::endl;


    cv::Mat result = pipe.getDescriptor()->getImage();
    imwrite("result.jpg", result);

    AIVision::deinit();

    return 0;
}
