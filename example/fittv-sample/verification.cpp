/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/Pipe.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <filesystem> // C++17

#include "Renderer.h"
#include <aif/bodyPoseEstimation/fittv/FitTvPoseDescriptor.h>

using namespace aif;
//namespace rj = rapidjson;
namespace fs = std::filesystem;

std::string savePath;

void drawResults(const std::string& inputPath, const std::string & outputPath, const Pipe& pipe)
{
    std::cout << "Input: " << inputPath << std::endl;
    std::cout << "Output: " << std::endl << pipe.getDescriptor()->getResult() << std::endl;
    auto fd = std::dynamic_pointer_cast<FitTvPoseDescriptor>(pipe.getDescriptor());
    cv::Mat result = fd->getImage();
    for (auto& keyPoints : fd->getKeyPoints()) {
        result = Renderer::drawPose2d(result, keyPoints, false);
    }

    cv::imwrite(outputPath, result);
}


std::string getImageName(const std::string& outputPath)
{
    size_t found = outputPath.find("/col");
    std::string img_name;
    if (found != std::string::npos) {
        img_name = outputPath.substr(found+1);
    } else {
        size_t found2 = outputPath.find("/");
        img_name = outputPath.substr(found2+1);
    }
    std::cout << "CHECKING image name for json: " << img_name << std::endl;

    return img_name;
}

bool detectFiles(Pipe& pipe, std::vector<cv::String>& files, bool saveToFiles)
{
    std::string jsonPath = savePath + "outputs.json";
    std::ofstream ofs(jsonPath);

    std::error_code ec;
    bool startSaving = false;
    for (int j = 0; j < files.size(); j++) {
        if (fs::is_directory(files[j], ec)) {
            std::cout << __LINE__ << " Check the input path name again : " << files[j] << " is a directory\n";
            continue;
        }

        if (files[j].rfind(".jpg") == cv::String::npos) {
            continue;
        }

        cv::Mat image = cv::imread(files[j]);
        if (!pipe.detect(image)) {
           std::cout << "failed to build pipe" << std::endl;
           return false;
        }

        std::string outputPath = files[j];
        size_t found = outputPath.find_last_of(".");
        if (found == std::string::npos) {
            std::cout << "Input Image file is wrong" << std::endl;
            return false;
        }

        std::string img_name = getImageName(files[j]);

        outputPath = outputPath.substr(0, found) + "_res" + outputPath.substr(found);
        // std::cout << "outImagePath: " << outputPath << std::endl;

        drawResults(files[j], outputPath, pipe);
        if (saveToFiles) {
            if (startSaving) {
                ofs << ",";
            }
            ofs << "\"" << img_name << "\":" << pipe.getDescriptor()->getResult();
            startSaving = true;
        }
    }

    ofs.close();

    return true;
}

void verify_Dataset(Pipe& pipe, const std::string& inputPath, bool saveToFiles)
{
    if (inputPath.find("pose_model_val_data") != std::string::npos) {
        /* col9_pose_model_val_data/images/ */
        for (auto i = 1; i < 10; i++) {
            std::string inputPathDir(inputPath);
            inputPathDir += std::to_string(i);
            inputPathDir += "/";
            std::vector<cv::String> files;
            cv::glob( inputPathDir + "*.jpg", files );  /* files[] are only .jpg file. */

            savePath = inputPathDir;

            if (detectFiles(pipe, files, saveToFiles) == false) {
                return;
            }
        }
    } else {
        /* col7_bodyscan/
           col8_bodyscan_2/
           col10_stop/images/ */

        std::vector<cv::String> files;
        cv::glob( inputPath + "*.jpg", files );        /* files[] are only .jpg file. */

        savePath = inputPath;

        if (detectFiles(pipe, files, saveToFiles) == false) {
            return;
        }
    }
}

