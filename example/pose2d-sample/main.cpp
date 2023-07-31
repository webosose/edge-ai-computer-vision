/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDescriptor.h>
#include <Renderer.h>

#include <opencv2/opencv.hpp>

namespace rj = rapidjson;
using namespace aif;

int main(int argc, char* argv[])
{
    std::string option(R"(
        {
            "common": {
              "useXnnpack": true,
              "numThreads": 1
            },
            "delegates": [
              {
                "name": "npu_delegate",
                "option": {}
              }
            ]
        })");

    AIVision::init();

    cv::Mat img = cv::imread("/usr/share/aif/images/person.jpg");
    std::string id = "pose2d_resnet_npu";

    auto detector = DetectorFactory::get().getDetector(id, option);
    auto descriptor = DetectorFactory::get().getDescriptor(id);
    detector->detect(img, descriptor);
    std::cout << descriptor->toStr() << std::endl;

    cv::Mat result = Renderer::drawPose2d(img, std::dynamic_pointer_cast<Pose2dDescriptor>(descriptor)->getKeyPoints());
    cv::imwrite("./result_pose2d.jpg", result);

    AIVision::deinit();
    return 0;
}

