/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/facade/EdgeAIVision.h>
#include <aif/base/AIVision.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <gtest/gtest.h>

using namespace aif;
namespace rj = rapidjson;

class ExtraPipeApiFacadeTest : public ::testing::Test
{
protected:
    ExtraPipeApiFacadeTest() = default;
    ~ExtraPipeApiFacadeTest() = default;
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

    std::string facePipeId = "pipe_face";
    std::string faceNodeId = "detect_face";
    std::string facePipeConfig = R"(
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
                            "model": "face_yunet_360_640"
                        }
                    }
                }
            ]
        }
    )";
    std::string facePipeNewConfig = R"(
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
                            "model": "face_yunet_360_640",
                            "param": {
                                "modelParam": {
                                    "nmsThreshold": 0.5
                                }
                            }
                        }
                    }
                }
            ]
        }
    )";
    std::string facePipeInvalidConfig = R"(
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
                            "model": "face_yunet_360_640",
                            "param": {
                                "autoDelegate": {
                                    "policy": "MIN_LATENCY"
                                }
                            }
                        }
                    }
                }
            ]
        }
    )";

    std::string hgPipeId= "pipe_hand_gesture";
    std::string hgPipeConfig = R"(
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
                    "id" : "hand_landmark",
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
};

TEST_F(ExtraPipeApiFacadeTest, pipeCreateTest_multiple_pipes)
{
    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ASSERT_TRUE(ai.startup());
    ASSERT_TRUE(ai.isStarted());
    EXPECT_TRUE(ai.pipeCreate(facePipeId, facePipeConfig));
    EXPECT_TRUE(ai.pipeCreate(hgPipeId, hgPipeConfig));
    EXPECT_TRUE(ai.pipeDelete(facePipeId));
    EXPECT_TRUE(ai.pipeDelete(hgPipeId));
    EXPECT_TRUE(ai.shutdown());
}

TEST_F(ExtraPipeApiFacadeTest, pipeCreateTest_multiple_nodes)
{
    std::string basePath = AIVision::getBasePath();
    cv::Mat input = cv::imread(basePath + "/images/hand_left.jpg", cv::IMREAD_COLOR);
    std::string output;

    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ASSERT_TRUE(ai.startup());
    ASSERT_TRUE(ai.isStarted());
    EXPECT_TRUE(ai.pipeCreate(hgPipeId, hgPipeConfig));
    EXPECT_TRUE(ai.pipeDetect(hgPipeId, input, output));
    EXPECT_TRUE(ai.pipeDelete(hgPipeId));
    EXPECT_TRUE(ai.shutdown());

    rj::Document d;
    d.Parse(output.c_str());
    ASSERT_TRUE(d.IsObject());
    ASSERT_TRUE(d.HasMember("returnCode"));
    EXPECT_EQ(d["returnCode"].GetInt(), 0);
    EXPECT_EQ(d["results"].GetArray().Size(), 3);
    ASSERT_TRUE(d["results"].GetArray()[0].HasMember("detect_palm"));
    ASSERT_TRUE(d["results"].GetArray()[1].HasMember("palm_crop"));
    ASSERT_TRUE(d["results"].GetArray()[2].HasMember("hand_landmark"));
    ASSERT_TRUE(d["results"].GetArray()[2]["hand_landmark"].HasMember("hands"));
    EXPECT_EQ(d["results"].GetArray()[2]["hand_landmark"]["hands"].GetArray().Size(), 1);
}