/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <VideoTester.h>
#include <fstream>
#include <rapidjson/document.h>

#include "rppg_pipe.h"

using namespace aif;
namespace rj = rapidjson;

cv::Mat drawResult(cv::Mat src, std::string &output_value, bool realTime_output)
{
    std::string str_1, str_2, str_3;
    if (output_value == "0")
    {
        str_1 = "Stabilization in progress";
        str_2 = "rPPG HR = --";
    }
    else
    {
        std::string result_signalCondition, result_bpm;

        rj::Document d;
        d.Parse(output_value.c_str());
        for (auto &values : d["rPPG"].GetArray())
        {
            result_signalCondition = values["signalCondition"].GetString();
            int int_bpm            = static_cast<int>(std::round(values["bpm"].GetFloat()));
            result_bpm             = std::to_string(int_bpm);
        }

        if (result_signalCondition == "Bad")
        {
            str_1 = "Signal unstable";
            str_2 = "rPPG HR = --";
        }
        else if (result_signalCondition == "Normal")
        {
            str_1 = "HR Calculation";
            str_2 = "rPPG HR = " + result_bpm + " bpm";
        }
    }

    if (realTime_output)
    {
        str_3 = "SWP, RealTime BPM";
        cv::putText(src, str_3, cv::Point(int(src.cols * 0.25), int(src.rows * 0.05)),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
        cv::putText(src, str_1, cv::Point(int(src.cols * 0.25), int(src.rows * 0.1)),
                    cv::FONT_HERSHEY_SIMPLEX, 1.1, cv::Scalar(0, 100, 0), 2);
        cv::putText(src, str_2, cv::Point(int(src.cols * 0.25), int(src.rows * 0.2)),
                    cv::FONT_HERSHEY_SIMPLEX, 1.1, cv::Scalar(0, 0, 255), 2);
    }
    else
    {
        str_3 = "SWP, Pre-run BPM";
        cv::putText(src, str_3, cv::Point(int(src.cols * 0.5), int(src.rows * 0.05)),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
        cv::putText(src, str_1, cv::Point(int(src.cols * 0.5), int(src.rows * 0.1)),
                    cv::FONT_HERSHEY_SIMPLEX, 1.1, cv::Scalar(0, 100, 0), 2);
        cv::putText(src, str_2, cv::Point(int(src.cols * 0.5), int(src.rows * 0.2)),
                    cv::FONT_HERSHEY_SIMPLEX, 1.1, cv::Scalar(0, 0, 255), 2);
    }

    return src;
}

int main(int argc, char *argv[])
{
    RppgConfig config;
    config.dataPipeId     = "rppg_pipe_data";
    config.dataPipeConfig = R"(
        {
            "name": "rppg_pipe_data",
            "nodes": [
                {
                    "id": "detect_face",
                    "input": ["image"],
                    "output": ["image", "inference"],
                    "operation": {
                        "type": "detector",
                        "config": {
                            "model": "person_yolov3_v2_npu",
                            "param": {
                                "modelParam": {"detectObject": "FACE"},
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
                    "id": "extract_face_mesh_rgb",
                    "input": ["image","inference"],
                    "output": ["image"],
                    "operation": {
                        "type": "face_mesh_rgb_extract",
                        "config": {"targetId": "face_mesh"}
                    }
                }
            ]
        }
    )";

    config.inferencePipeId     = "rppg_pipe_inference";
    config.inferencePipeConfig = R"(
        {
            "name": "rppg_pipe_inference",
            "descriptor" : "rppg_inference_pipe",
            "nodes": [
                {
                    "id": "rppg_pre_process",
                    "input": ["image"],
                    "output": ["image"],
                    "operation": {
                        "type": "rppg_pre_process",
                        "config": {}
                    }
                },
                {
                    "id": "detect_rppg",
                    "input": ["image"],
                    "output": ["image", "inference"],
                    "operation": {
                        "type": "detector",
                        "config": {
                            "model": "rppg_cpu",
                            "param": {
                                "autoDelegate": {"policy": "CPU_ONLY"}
                            }
                        }
                    }
                },
                {
                    "id": "change_input",
                    "input": ["image","inference"],
                    "output": ["image","inference"],
                    "operation": {
                        "type": "change_input",
                        "config": {"changeRect": [200, 0, 200, 1]}
                    }
                },
                {
                    "id": "detect_rppg2",
                    "input": ["image", "inference"],
                    "output": ["image", "inference"],
                    "operation": {
                        "type": "detector",
                        "config": {
                            "model": "rppg_cpu",
                            "param": {
                                "autoDelegate": {"policy": "CPU_ONLY"}
                            }
                        }
                    }
                },
                {
                    "id": "rppg_post_process",
                    "input": ["image","inference"],
                    "output": ["image","inference"],
                    "operation": {
                        "type": "rppg_post_process",
                        "config": {}
                    }
                }
            ]
        }
    )";

    config.dataAggregationSize = 15 * 8;                          // 120 = 15 frame * 8 seconds
    config.inferenceInterval   = std::chrono::milliseconds(1000); // 1000 ms

    std::map<int, std::string> result;
    std::string last_json = "0";
    config.onResultFunc   = [&](int id, const std::string &json) {
        std::cout << "------------------------------------------------------------------"
                  << std::endl;
        std::cout << "Result : " << id << ": " << 4.0f + (float)(id * 0.066f) << std::endl;
        std::cout << json << std::endl;
        result[id] = json;
        last_json  = json;
    };

    size_t frameCount           = 10; // for sample app test video frames
    std::string inputPath       = R"(/usr/share/aif/images/rppg_input.mp4)";
    std::string outputPath      = "./output.mp4";
    std::string finalOutputPath = "./output_final.mp4";

    if (argc >= 2)
    {
        frameCount = std::stoi(argv[1]);
    }

    std::string name;
    if (argc >= 3)
    {
        name            = std::string(argv[2]);
        inputPath       = R"(./)" + name + ".mp4";
        outputPath      = R"(./)" + name + "_output.mp4";
        finalOutputPath = R"(./)" + name + "_output_final.mp4";
    }

    RppgPipe pipe;
    if (!pipe.startup(config))
    {
        std::cout << "rppg pipe startup failed" << std::endl;
    }

    int frameId = 0;
    VideoTester vt;
    vt.testWithFrameCount(inputPath, outputPath,
                          [&](const cv::Mat &src) -> cv::Mat {
                              std::cout << ++frameId << " : new frame  -------------------- "
                                        << std::endl;
                              if (!pipe.detect(frameId, (double)1 / vt.getFps(), src))
                              {
                                  std::cout << "rppg pipe pushImage failed" << std::endl;
                              }
                              return drawResult(src, last_json, true);
                          },
                          frameCount);

    if (!pipe.shutdown())
    {
        std::cout << "rppg pipe shutdown failed" << std::endl;
    }

    frameId = 0;
    std::string output_value = "0";
    vt.testWithFrameCount(outputPath, finalOutputPath,
            [&](const cv::Mat& src) -> cv::Mat {
                ++frameId;
                if (result.find(frameId) != result.end()) {
                    output_value = result[frameId];
                }
                return drawResult(src, output_value, false);
            }, frameCount);

    return 0;
}
