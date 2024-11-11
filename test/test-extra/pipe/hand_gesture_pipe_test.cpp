/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/Pipe.h>
#include <aif/base/AIVision.h>
#include <gtest/gtest.h>
#include "pipe/ConfigUtil.h"

#include <opencv2/opencv.hpp>
using namespace std;
using namespace aif;

class HandGesturePipeTest : public ::testing::Test
{
protected:
    HandGesturePipeTest() = default;
    ~HandGesturePipeTest() = default;

    static void SetUpTestCase() {
        AIVision::init();
    }

    static void TearDownTestCase() {
        AIVision::deinit();
    }
    void SetUp() override {
        basePath = AIVision::getBasePath();
    }

    void TearDown() override {
    }

public:
    std::string basePath;
    std::string palmDetectorPipe = R"(
        {
            "name" : "pipe_palm",
            "nodes" : [
                {
                    "id" : "detect_palm",
                    "input" : ["image"],
                    "output" : ["image", "inference"],
                    "operation" : {
                        "type" : "detector",
                        "config": {
                            "model": "palm_lite_cpu",
                            "param": {
                                "autoDelegate": {
                                    "policy": "CPU_ONLY"
                                }
                            }
                        }
                    }
                },
                {
                    "id" : "palm_crop",
                    "input" : ["image", "inference"],
                    "output" : ["image", "inference"],
                    "operation" : {
                        "type" : "palm_crop",
                        "config": {
                            "targetId" : "detect_palm"
                        }
                    }
                }
            ]
        }
    )";
    std::string handGesturePipe = R"(
        {
            "name" : "pipe_hand_gesture",
            "nodes": [
                {
                    "id" : "detect_palm",
                    "input" : ["image"],
                    "output" : ["image", "inference"],
                    "operation" : {
                        "type" : "detector",
                        "config": {
                            "model": "palm_lite_cpu",
                            "param": {
                                "autoDelegate": {
                                    "policy": "CPU_ONLY"
                                }
                            }
                        }
                    }
                },
                {
                    "id" : "palm_crop",
                    "input" : ["image", "inference"],
                    "output" : ["image", "inference"],
                    "operation" : {
                        "type" : "palm_crop",
                        "config": {
                            "targetId" : "detect_palm"
                        }
                    }
                },
                {
                    "id" : "detect_hand_landmark",
                    "input" : ["image", "inference"],
                    "output" : ["image", "inference"],
                    "operation" : {
                        "type" : "detector",
                        "config": {
                            "model": "handlandmark_lite_cpu",
                            "param": {
                                "autoDelegate": {
                                    "policy": "CPU_ONLY"
                                }
                            }
                        }
                    }
                }
            ]
        }
    )";
    std::string invalidPipe1= R"(
        {
            "nodes": [
                {
                    "id" : "detect_face",
                    "input" : ["image"],
                    "output" : ["image", "inference"],
                    "operation" : {
                        "type" : "detector",
                        "config": {
                            "model": "face_yunet_360_640"
                        }
                    }
                }
            ]
        }
    )";

};

TEST_F(HandGesturePipeTest, 01_build_hand_gesture)
{
    Pipe pipe1;
    ASSERT_TRUE(pipe1.build(palmDetectorPipe));
    EXPECT_EQ(pipe1.getName(), "pipe_palm");

    Pipe pipe2;
    ASSERT_TRUE(pipe2.build(handGesturePipe));
    EXPECT_EQ(pipe2.getName(), "pipe_hand_gesture");
}

TEST_F(HandGesturePipeTest, 02_detect_hand_gesture)
{
    cv::Mat image = cv::imread(basePath + "/images/hand_right.jpg");
    Pipe pipe;
    ASSERT_TRUE(pipe.build(handGesturePipe));
    ASSERT_TRUE(pipe.detect(image));

    auto descriptor = pipe.getDescriptor();
    ASSERT_TRUE(descriptor != nullptr);

    string resultPalms = descriptor->getResult("detect_palm");
    rj::Document d1;
    d1.Parse(resultPalms.c_str());
    EXPECT_TRUE(d1["palms"].Size() == 1);

    string resultCrop = descriptor->getResult("palm_crop");
    rj::Document d2;
    d2.Parse(resultCrop.c_str());
    EXPECT_TRUE(d2["region"].Size() == 4);

    string resultHandLandmark = descriptor->getResult("detect_hand_landmark");
    rj::Document d3;
    d3.Parse(resultHandLandmark.c_str());
    EXPECT_TRUE(d3["hands"].Size() == 1);
}
