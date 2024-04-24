/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <VideoTester.h>
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

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
        cv::putText(src, str_3, cv::Point(int(src.cols * 0.01), int(src.rows * 0.05)),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
        cv::putText(src, str_1, cv::Point(int(src.cols * 0.01), int(src.rows * 0.1)),
                    cv::FONT_HERSHEY_SIMPLEX, 1.1, cv::Scalar(0, 100, 0), 2);
        cv::putText(src, str_2, cv::Point(int(src.cols * 0.01), int(src.rows * 0.2)),
                    cv::FONT_HERSHEY_SIMPLEX, 1.1, cv::Scalar(0, 0, 255), 2);
    }
    else
    {
        str_3 = "SWP, Pre-run BPM";
        cv::putText(src, str_3, cv::Point(int(src.cols * 0.4), int(src.rows * 0.05)),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
        cv::putText(src, str_1, cv::Point(int(src.cols * 0.4), int(src.rows * 0.1)),
                    cv::FONT_HERSHEY_SIMPLEX, 1.1, cv::Scalar(0, 100, 0), 2);
        cv::putText(src, str_2, cv::Point(int(src.cols * 0.4), int(src.rows * 0.2)),
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
                        "type": "face_mesh_extract",
                        "config": {
                            "targetId": "face_mesh",
                            "rgbExtractOn": true
                        }
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
//        std::cout << "------------------------------------------------------------------" << std::endl;
//        std::cout << "Result : " << id << ": " << 4.0f + (float)(id * 0.066f) << std::endl;
        std::cout << id << " : " << json << std::endl;
        result[id] = json;
        last_json  = json;
    };

    size_t frameCount           = 10; // for sample app test video frames
    std::string inputPath       = R"(/usr/share/aif/images/rppg_input.mp4)";
    std::string outputPath      = "./output.mp4";
    std::string finalOutputPath = "./output_final.mp4";

    if (argc >= 2) {
         frameCount = std::stoi(argv[1]);
        std::cout << "Usage # 1: ./apis-rppg-sample <number-of-frames>" << std::endl;
    }

    std::string dataConfigPath;
    std::string inferenceConfigPath;
    if (argc >= 3) {
        std::cout << "Usage # 2: ./apis-rppg-sample <number-of-frames> <rppg-pipe-data-config-file-path> <rppg-pipe-inference-config-file-path>" << std::endl;
        dataConfigPath = std::string(argv[2]);
        inferenceConfigPath = std::string(argv[3]);
    }

    std::string name;
    if (argc >= 5) {
        std::cout << "Usage # 3: ./apis-rppg-sample <number-of-frames> <rppg-pipe-data-config-file-path> <rppg-pipe-inference-config-file-path> "
                  << "<name-of-input-file-without .mp4>" << std::endl;
        name = std::string(argv[4]);
        inputPath = R"(./)" + name + ".mp4";
        outputPath = R"(./)" + name + "_output.mp4";
         finalOutputPath = R"(./)" + name + "_output_final.mp4";
     }

    bool useTimeInterval = false;
    std::string timeIntervalPath  = "./rppgTimeInterval.json";
    std::string timeInvName;
    if (argc >= 6) {
        std::cout << "Usage # 3: ./apis-rppg-sample <number-of-frames> <rppg-pipe-data-config-file-path> <rppg-pipe-inference-config-file-path> "
                  << "<name-of-input-file-without .mp4> <time-interval-json>" << std::endl;
        timeIntervalPath = std::string(argv[5]);
        useTimeInterval = true;
    }

    if(!dataConfigPath.empty()) {
        std::ifstream ifs(dataConfigPath);
        if (!ifs.is_open()) {
            std::cout << "failed to read file : " << dataConfigPath << std::endl;
            return 0;
        }
        config.dataPipeConfig.clear();
        std::string line;
        while (getline(ifs, line)){
            config.dataPipeConfig.append("\n");
            config.dataPipeConfig.append(line);
        }
    }

    if(!inferenceConfigPath.empty()) {
        std::ifstream ifs(inferenceConfigPath);
        if (!ifs.is_open()) {
            std::cout << "failed to read file : " << inferenceConfigPath << std::endl;
            return 0;
        }
        config.inferencePipeConfig.clear();
        std::string inf_line;
        while (getline(ifs, inf_line)){
            config.inferencePipeConfig.append("\n");
            config.inferencePipeConfig.append(inf_line);
        }
    }

    RppgPipe pipe;
    if (!pipe.startup(config))
    {
        std::cout << "rppg pipe startup failed" << std::endl;
    }

    std::vector<double> timeInterval;
    if (useTimeInterval) {
        std::ifstream ifs(timeIntervalPath);
        if (!ifs.is_open()) {
            std::cout << "failed to read file : " << timeIntervalPath << std::endl;
            return 0;
        }
        std::string str_time;
        std::string time_line;
        while (getline(ifs, time_line)){
            str_time.append(time_line);
        }

        size_t pos = 0;
        std::string token;
        std::string delimiter = ",";
        while ((pos = str_time.find(delimiter)) != std::string::npos) {
            token = str_time.substr(0, pos);
            timeInterval.push_back(stod(token));
            str_time.erase(0, pos + delimiter.length());
        }
    }

    // Read with GT Results ----------------------------------------------------------------------------------------------------------------------------
    std::string pyResultPath  = "./HR_result.json";
    std::string ppgResultPath  = "./HR_ref_PPG.json";
    std::vector<double> py_results, ppg_results;
    bool use_gt = false;
    if (argc >= 7) {
        std::cout << "Usage # 4: ./apis-rppg-sample <number-of-frames> <rppg-pipe-data-config-file-path> <rppg-pipe-inference-config-file-path> "
                  << "<name-of-input-file-without .mp4> <time-interval-json> <python-results-json-path> <PPG-results-json-path>" << std::endl;
        pyResultPath = std::string(argv[6]);
        ppgResultPath = std::string(argv[7]);
        use_gt = true;
    }

    if (use_gt) {
        rj::Document doc;
        // Read with python results
        std::ifstream ifs_py(pyResultPath);
        if (!ifs_py.is_open()) {
            std::cout << "failed to read file : " << pyResultPath << std::endl;
            return 0;
        }
        std::string pyRe;
        std::string line_py;
        while (getline(ifs_py, line_py)){
            pyRe.append("\n");
            pyRe.append(line_py);
        }

        doc.Parse(pyRe.c_str());
        for (auto& values : doc.GetObject())
        {
            py_results.push_back(values.value.GetFloat());
            // std::cout << values.name.GetString() << " : " << values.value.GetFloat()<< std::endl;
        }

        // Read with PPG results
        std::ifstream ifs_ppg(ppgResultPath);
        if (!ifs_ppg.is_open()) {
            std::cout << "failed to read file : " << ppgResultPath << std::endl;
            return 0;
        }
        std::string ppgRe;
        std::string line_ppg;
        while (getline(ifs_ppg, line_ppg)){
            ppgRe.append("\n");
            ppgRe.append(line_ppg);
        }

        doc.Parse(ppgRe.c_str());
        for (auto& values : doc.GetObject())
        {
            ppg_results.push_back(values.value.GetFloat());
            // std::cout << values.name.GetString() << " : " << values.value.GetFloat()<< std::endl;
        }
    }
    // ----------------------------------------------------------------------------------------------------------------------------------------------------


    int fps = 15; // 15fps
    int frameId = 0;
    VideoTester vt;
    double past_time = 0.0;
    double input_timeInterval = 0.0;
    vt.testWithFrameCount(inputPath, outputPath,
                          [&](const cv::Mat &src) -> cv::Mat {
                              ++frameId;
                              if(useTimeInterval) {
                                input_timeInterval = timeInterval[frameId - 1] - past_time;
                                past_time = timeInterval[frameId - 1];
                                // std::cout << "input_timeInterval : " << input_timeInterval << std::endl;
                                if (!pipe.detect(frameId, input_timeInterval, src)) {
                                    std::cout << "rppg pipe pushImage failed" << std::endl;
                                }
                              }
                              else {
                                if (!pipe.detect(frameId, (double)1/vt.getFps(), src)) {
                                    std::cout << "rppg pipe pushImage failed" << std::endl;
                                }
                              }

                              if ((frameId >= (int)config.dataAggregationSize) &&
                                  (frameId % fps) == 0)
                              {
                                if (!pipe.inference()) {
                                    std::cout << "rppg pipe infernece failed" << std::endl;
                                }
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

    // Compare with GT Results ----------------------------------------------------------------------------------------------------------------------------
    if (use_gt) {
        std::vector<double> rppg_results;
        rj::Document d;
        for (auto& r:result) {
            if(r.first != -1){
                // std::cout << "Checking here!!!!! " << r.first << ", " << r.second << std::endl;
                d.Parse(r.second.c_str());
                for (auto& values : d["rPPG"].GetArray()) {
                    rppg_results.push_back(values["bpm"].GetFloat());
                }
            }
        }
        std::cout << "Checking Length : " << py_results.size() << ", "<< ppg_results.size() << ", " << rppg_results.size() << std::endl;

        int len_results = rppg_results.size();
        double sum_rppg = 0.0;  // Compare with python results
        double abs_sumPy = 0.0;  // Compare with python results
        double abs_sumPPG = 0.0; // Compare with PPG results
        double abs_gtPPG = 0.0; // Compare python results with PPG results
        for (int i = 0; i < len_results; i++){
            // std::cout << "Checking values : " << i << " : "<< rppg_results[i] << ", "<< py_results[i] << ", " << ppg_results[i] << std::endl;
            double comPy  = rppg_results[i] - py_results[i];
            double comPPG = rppg_results[i] - ppg_results[i];
            double comgt = py_results[i] - ppg_results[i];
            abs_sumPy  += std::abs(comPy);
            abs_sumPPG += std::abs(comPPG);
            abs_gtPPG += std::abs(comgt);
            sum_rppg  += rppg_results[i];
        }

        auto avg_ttsum_rppg  = sum_rppg  / len_results;
        double vari_rppg  = 0.0;
        for (int i = 0; i < len_results; i++){
            vari_rppg  += std::pow(rppg_results[i] - avg_ttsum_rppg , 2);
        }
        vari_rppg  /= len_results;

        std::cout << "Only rPPG Results: " << std::endl;
        for (int i = 0; i < len_results; i++){
            std::cout << rppg_results[i] << std::endl;
        }

        double sum_pys = 0.0;
        for (int i = 0; i < len_results; i++){
            sum_pys  += py_results[i];
        }
        auto avg_ttsum_pys  = sum_pys  / len_results;
        double vari_pys  = 0.0;
        for (int i = 0; i < len_results; i++){
            vari_pys  += std::pow(py_results[i] - avg_ttsum_pys , 2);
        }
        vari_pys  /= len_results;

        std::cout << "Mean Absolute Error (Python vs. rppg bpm) : " << (abs_sumPy  / len_results) << std::endl;
        std::cout << "Mean Absolute Error (PPG vs. rppg bpm)    : " << (abs_sumPPG / len_results) <<  std::endl;
        std::cout << "Mean Absolute Error (Python vs. PPG bpm) : " << (abs_gtPPG / len_results) <<  std::endl;
        std::cout << "STDEV : "<< std::sqrt(vari_rppg) << std::endl;
        std::cout << "STDEV PYS : "<< std::sqrt(vari_pys) << std::endl;
    }
    // ----------------------------------------------------------------------------------------------------------------------------------------------------

    return 0;
}
