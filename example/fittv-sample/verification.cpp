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
namespace fs = std::filesystem;

std::string savePath;
void drawResults(const std::string& inputPath, const std::string & outputPath, const Pipe& pipe, bool drawAll = true)
{
    std::cout << "Input: " << inputPath << std::endl;
    std::cout << "Output: " << std::endl << pipe.getDescriptor()->getResult() << std::endl;
    auto fd = std::dynamic_pointer_cast<FitTvPoseDescriptor>(pipe.getDescriptor());
    cv::Mat result = fd->getImage();
    for (auto& keyPoints : fd->getKeyPoints()) {
        result = Renderer::drawPose2d(result, keyPoints, drawAll);
    }

    if (drawAll) {
        const cv::Rect &roiRect = fd->getRoiRect();
        auto cropBbox = fd->getCropBbox(); // get fixedBbox
        auto cropScale = fd->getCropData(); // get scale applied to fixedBbox.

        /* cropRect in here, is approximation of real cropRect, because it uses affine transformation */
        std::vector<cv::Rect> cropRects;
        for (int i = 0; i < cropBbox.size(); i++) {
            int crop_width = cropScale[i].x * 200.f;
            int crop_height = cropScale[i].y * 200.f;
            int crop_xmin = cropBbox[i].c_x - (crop_width / 2);
            int crop_ymin = cropBbox[i].c_y - (crop_height / 2);
            auto cropRect = cv::Rect(crop_xmin + roiRect.x , crop_ymin + roiRect.y, crop_width, crop_height);
            cropRects.push_back(cropRect);
        }

        result = Renderer::drawRects(result, { roiRect }, cv::Scalar(127,127,127), 2);
        result = Renderer::drawRects(result, cropRects, cv::Scalar(255, 0, 0), 1);
        result = Renderer::drawBoxes(result, fd->getBboxes(), cv::Scalar(0, 0, 255), 2);
    }

    cv::imwrite(outputPath, result);
}

std::string getImageName(const std::string& outputPath, bool onlyFileName = false)
{
    std::string img_name;
    if (onlyFileName) {
        size_t found = outputPath.rfind("/");
        img_name = outputPath.substr(found+1);
        return img_name;
    }

    size_t found = outputPath.find("/col");
    if (found != std::string::npos) {
        img_name = outputPath.substr(found+1);
    } else {
        size_t found2 = outputPath.find("/u0"); // u007 u011 u016 u019
        if (found2 != std::string::npos) {
            img_name = outputPath.substr(found2+1);
        } else {
            size_t found3 = outputPath.rfind("/");
            img_name = outputPath.substr(found3+1);
        }
    }
    std::cout << "CHECKING image name for json: " << img_name << std::endl;

    return img_name;
}

bool checkDetectedFileName(const std::string &inputFileName, const std::string &result)
{
    rj::Document json;
    json.Parse(result);
    if(!json.HasMember("poseEstimation")) {
        return true;
    }

    const rj::Value &res = json["poseEstimation"][0];
    if (res.HasMember("dbg_fname")) {
        const rj::Value &dbg_fname = json["poseEstimation"][0]["dbg_fname"];
        auto fname = dbg_fname.GetString();
        if (fname == inputFileName) {
            std::cout << __func__<< " input File Name is SAME with detected File Name " << inputFileName << " == " << fname << std::endl;
            return true;
        } else {
            std::cout << __func__<< " input File Name is different with detected File Name " << inputFileName << " != " << fname << std::endl;
            return false;
        }
    }

    return true;
}

bool detectFiles(Pipe& pipe, std::vector<cv::String>& files, bool saveToFiles, bool saveToImages)
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
           std::cout << "failed to detect" << std::endl;
        }

        const std::string& imgFileName = getImageName(files[j], true); // only file name

        if (!checkDetectedFileName(imgFileName, pipe.getDescriptor()->getResult())) {
            std::cout << " Detect Wrong File\n";
            return false;
        }

        size_t found = imgFileName.find_last_of(".");
        if (found == std::string::npos) {
            std::cout << "Input Image file is wrong" << std::endl;
            return false;
        }
        std::string outputPath = savePath + "/" + imgFileName.substr(0, found) + "_res" + imgFileName.substr(found);
        std::cout << "outImagePath: " << outputPath << std::endl;

        std::string imgNameKey = getImageName(files[j]); // with dir path..

        if (saveToFiles) {
            if (startSaving) {
                ofs << ",";
            }
            ofs << "\"" << imgNameKey << "\":" << pipe.getDescriptor()->getResult();
            startSaving = true;
        }
        if (saveToImages) {
            drawResults(files[j], outputPath, pipe, true);
        }
    }

    ofs.close();

    return true;
}

void verify_Dataset(Pipe& pipe, const std::string& inputPath, const std::string& outputPath)
{
    std::vector<cv::String> files;
    cv::glob( inputPath + "*.jpg", files );        /* files[] are only .jpg file. */
    savePath = outputPath;

    if (inputPath.find("/u0") != std::string::npos) {
        /* u007
           u011
           u016
           u019 */

        std::sort(files.begin(), files.end());

        if (detectFiles(pipe, files, true, false) == false) {
            return;
        }
    } else {
        /* col7_bodyscan/
           col8_bodyscan_2/
           col9_pose_model_val_data/images/$id/
           col10_stop/images/ */

        if (detectFiles(pipe, files, true, true) == false) {
            return;
        }
    }
}

