/*
 * Copyright (c) 2022-2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/facade/EdgeAIVision.h>
#include <aif/tools/PerformanceReporter.h>
#include <rapidjson/document.h>
#include <opencv2/opencv.hpp>
#include <aif/tools/Utils.h>
#include <VideoTester.h>

using namespace aif;
namespace rj = rapidjson;

cv::Mat drawMask(int width, int height, int maskW, int maskH, uint8_t* mask)
{
    //just draw!
    cv::Mat maskImage = cv::Mat(maskH, maskW, CV_8UC1, mask); // smaller than 240 x 135 (maximum)

    cv::Mat scaledImage;
    cv::resize(maskImage, scaledImage, cv::Size(width, height), 0, 0, cv::INTER_LINEAR);
    scaledImage.convertTo(scaledImage, CV_32FC1, 1.0/255);

    return scaledImage;
}

cv::Mat alphaBlending(cv::Mat &bgImage, cv::Mat fgImage, cv::Mat &alpha)
{
    cv::Mat results = cv::Mat::zeros(bgImage.size(), bgImage.type());

    bgImage.convertTo(bgImage, CV_32FC3);
    fgImage.convertTo(fgImage, CV_32FC3);
    cv::Mat alpha3;
    cv::cvtColor(alpha, alpha3, cv::COLOR_GRAY2BGR);

    cv::multiply(fgImage, alpha3, fgImage);

    if (bgImage.size() != fgImage.size() ) { // add it to right side!
        cv::Mat fgDst, alpha3Dst;
        cv::copyMakeBorder(fgImage, fgDst, 0, bgImage.rows - fgImage.rows, bgImage.cols - fgImage.cols, 0, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
        cv::copyMakeBorder(alpha3, alpha3Dst, 0, bgImage.rows - alpha3.rows, bgImage.cols - alpha3.cols, 0, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

        cv::multiply(bgImage, cv::Scalar::all(1.0) - alpha3Dst, bgImage);
        cv::add(fgDst, bgImage, results);
    } else {
        cv::multiply(bgImage, cv::Scalar::all(1.0) - alpha3, bgImage);
        cv::add(fgImage, bgImage, results);
    }
    return results;
}

cv::Mat drawResults(const std::string &output, const ExtraOutput &extraOutput, const cv::Mat &fgImage, bool imageDup)
{
    cv::Mat bgImage = (imageDup) ? fgImage : cv::imread("/usr/share/aif/images/beach.jpg", cv::IMREAD_COLOR);
    cv::Mat res;

    // std::cout << output << std:: endl;
    rj::Document d;
    d.Parse(output.c_str());

    cv::Mat masking;
    const rj::Value &segments = d["segments"];
    for (rj::SizeType i = 0; i < segments.Size(); i++) { // maybe always 1
        const auto &inputRect = segments[i]["inputRect"].GetArray();
        int x = inputRect[0].GetInt();
        int y = inputRect[1].GetInt();
        int width = inputRect[2].GetInt();
        int height = inputRect[3].GetInt();

        const auto &maskRect = segments[i]["maskRect"].GetArray();
        int maskW = maskRect[0].GetInt();
        int maskH = maskRect[1].GetInt();

        std::cout << "inputRect x: " << x << " y : " << y << " w: " << width << " h : " << height << std::endl;
        std::cout << "maskRect maskW: " << maskW << " maskH : " << maskH << std::endl;

        masking = drawMask(width, height, maskW, maskH, static_cast<uint8_t*>(extraOutput.buffer()));
        res = alphaBlending(bgImage, fgImage( cv::Rect(x, y, width, height) ), masking);
    }

    res.convertTo(res, CV_8U);
    return res;
}

int main(int argc, char** argv)
{
    std::cout << "Usage: ./segmentation-sample <input-file-path> <#iternum> <config-file-path> <#imageDup_1_or_0>" << std::endl;

    std::string inputPath = R"(/usr/share/aif/images/person.jpg)";
    std::string config = "";
    std::string configPath = "";

    int num = 1;
    bool isVideoInput = false;
    bool imageDup = false;

    if (argc >= 2) {
        inputPath = argv[1];
        if (inputPath.find(".mp4") != std::string::npos) {
            isVideoInput = true;
        }
    }

    if (argc >= 3) {
        num = std::stoi(argv[2]);
    }
    if (argc >= 4) {
        configPath = argv[3];
        config = fileToStr(configPath);
    }
    if (argc >= 5) {
        imageDup = (std::stoi(argv[4]) > 0) ? true : false;
    }

    std::string outputPath = (isVideoInput) ? R"(./output.mp4)" : R"(./res.jpg)";

    std::cout << "input : " << inputPath << std::endl;
    std::cout << "config : " << config << std::endl;
    std::cout << "output: " << outputPath << std::endl;

    EdgeAIVision::DetectorType type = EdgeAIVision::DetectorType::SEGMENTATION;
    EdgeAIVision& ai = EdgeAIVision::getInstance();

    std::unique_ptr<uint8_t[]> maskingData{ new uint8_t[1280 * 720]}; /* ROI or full camera size ...*/
    ExtraOutput extraOutput(
            ExtraOutputType::UINT8_ARRAY,
            static_cast<void*>(maskingData.get()),
            sizeof(uint8_t) * 1280 * 720);

    ai.startup();
    ai.createDetector(type, config);

    if (!isVideoInput) {
        cv::Mat input = cv::imread(inputPath, cv::IMREAD_COLOR);
        std::string output;
        for (int i = 0; i < num; i++) {
            ai.detect(type, input, output, extraOutput);
            cv::Mat results = drawResults(output, extraOutput, input, false);
            cv::imwrite("./alphaBlending.jpg", results);
        }
    } else {
        VideoTester vt;
#if 0
        vt.test(inputPath, outputPath,
                [&](const cv::Mat& src) -> cv::Mat {
                    std::string output;
                    ai.detect(type, src, output, extraOutput);
                    return drawResults(output, extraOutput, src);
                }
        );
#else
        unsigned int frameCount = num;
        vt.testWithFrameCount(inputPath, outputPath,
                [&](const cv::Mat& src) -> cv::Mat {
                    std::string output;
                    ai.detect(type, src, output, extraOutput);
                    return drawResults(output, extraOutput, src, imageDup);
                }, frameCount
        );

#endif
    }
    PerformanceReporter::get().showReport();

    ai.deleteDetector(type);
    ai.shutdown();

    return 0;
}