/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/Pipe.h>
#include <aif/base/AIVision.h>
#include <aif/tools/Utils.h>
#include "ConfigUtil.h"

#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace aif;

class PipeTest : public ::testing::Test
{
protected:
    PipeTest() = default;
    ~PipeTest() = default;

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
    std::string facePipe = R"(
        {
            "name" : "pipe_face",
            "nodes": [
                {
                    "id" : "detect_face",
                    "input" : ["image"],
                    "output" : ["image", "inference"],
                    "operation" : {
                        "type" : "detector",
                        "config": {
                            "model": "face_yunet_cpu"
                        }
                    }
                }
            ]
        }
    )";
    std::string invalidPipe = R"(
        {
            "nodes": [
                {
                    "id" : "detect_face",
                    "input" : ["image"],
                    "output" : ["image", "inference"],
                    "operation" : {
                        "type" : "detector",
                        "config": {
                            "model": "face_yunet_cpu"
                        }
                    }
                }
            ]
        }
    )";

};

TEST_F(PipeTest, 01_build)
{
    Pipe pipe;
    EXPECT_TRUE(pipe.build(facePipe));
    EXPECT_EQ(pipe.getName(), "pipe_face");
}

TEST_F(PipeTest, 02_build_invalid)
{
    Pipe pipe;
    EXPECT_FALSE(pipe.build(invalidPipe));
}

TEST_F(PipeTest, 03_detect_face)
{
    cv::Mat image = cv::imread(basePath + "/images/person.jpg");
    Pipe pipe;
    EXPECT_TRUE(pipe.build(facePipe));
    EXPECT_TRUE(pipe.detect(image));

    auto descriptor = pipe.getDescriptor();
    EXPECT_TRUE(descriptor != nullptr);

    string result = descriptor->getResult("detect_face");
    rj::Document d;
    d.Parse(result.c_str());
    EXPECT_TRUE(d["faces"].Size() == 1);
}

TEST_F(PipeTest, 04_detectFromFile_face)
{
    Pipe pipe;
    EXPECT_TRUE(pipe.build(facePipe));
    EXPECT_TRUE(pipe.detectFromFile(basePath + "/images/person.jpg"));

    auto descriptor = pipe.getDescriptor();
    EXPECT_TRUE(descriptor != nullptr);

    string result = descriptor->getResult("detect_face");
    rj::Document d;
    d.Parse(result.c_str());
    EXPECT_TRUE(d["faces"].Size() == 1);
}

TEST_F(PipeTest, 05_detectFromBase64_face)
{
    std::string base64Image = aif::base64Encode(basePath + "/images/person.jpg");

    Pipe pipe;
    EXPECT_TRUE(pipe.build(facePipe));
    EXPECT_TRUE(pipe.detectFromBase64(base64Image));

    auto descriptor = pipe.getDescriptor();
    EXPECT_TRUE(descriptor != nullptr);

    string result = descriptor->getResult("detect_face");
    rj::Document d;
    d.Parse(result.c_str());
    EXPECT_TRUE(d["faces"].Size() == 1);
}
