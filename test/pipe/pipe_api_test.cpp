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

class PipeApiFacadeTest : public ::testing::Test
{
protected:
    PipeApiFacadeTest() = default;
    ~PipeApiFacadeTest() = default;
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

TEST_F(PipeApiFacadeTest, pipeCreateTest)
{
    EdgeAIVision& ai = EdgeAIVision::getInstance();
    EXPECT_FALSE(ai.isStarted());
    EXPECT_FALSE(ai.pipeCreate(facePipeId, facePipeConfig));

    ASSERT_TRUE(ai.startup());
    ASSERT_TRUE(ai.isStarted());
    EXPECT_TRUE(ai.pipeCreate(facePipeId, facePipeConfig));
    EXPECT_FALSE(ai.pipeCreate(facePipeId, facePipeConfig));
    EXPECT_TRUE(ai.shutdown());
}

TEST_F(PipeApiFacadeTest, pipeDeleteTest)
{
    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ASSERT_TRUE(ai.startup());
    ASSERT_TRUE(ai.isStarted());
    EXPECT_FALSE(ai.pipeDelete(facePipeId));
    EXPECT_TRUE(ai.pipeCreate(facePipeId, facePipeConfig));
    EXPECT_TRUE(ai.pipeDelete(facePipeId));
    EXPECT_FALSE(ai.pipeDelete(facePipeId));
    EXPECT_TRUE(ai.shutdown());
}

TEST_F(PipeApiFacadeTest, pipeUpdateTest)
{
    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ASSERT_TRUE(ai.startup());
    ASSERT_TRUE(ai.isStarted());
    EXPECT_TRUE(ai.pipeCreate(facePipeId, facePipeConfig));
    EXPECT_TRUE(ai.pipeUpdate(facePipeId, facePipeNewConfig));
    EXPECT_FALSE(ai.pipeUpdate(facePipeId, facePipeInvalidConfig));

    EXPECT_TRUE(ai.shutdown());
}

TEST_F(PipeApiFacadeTest, pipeDetectTest)
{
    std::string basePath = AIVision::getBasePath();
    cv::Mat input = cv::imread(basePath + "/images/person.jpg", cv::IMREAD_COLOR);
    std::string output;

    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ASSERT_TRUE(ai.startup());
    ASSERT_TRUE(ai.isStarted());
    EXPECT_TRUE(ai.pipeCreate(facePipeId, facePipeConfig));
    EXPECT_TRUE(ai.pipeDetect(facePipeId, input, output));
    EXPECT_TRUE(ai.pipeDelete(facePipeId));
    EXPECT_TRUE(ai.shutdown());

    rj::Document d;
    d.Parse(output.c_str());
    ASSERT_TRUE(d.IsObject());
    ASSERT_TRUE(d["results"].GetArray()[0][faceNodeId.c_str()].HasMember("faces"));
    EXPECT_EQ(d["results"].GetArray()[0][faceNodeId.c_str()]["faces"].Size(), 1);
}

TEST_F(PipeApiFacadeTest, pipeDetectFromFileTest)
{
    std::string inputPath = AIVision::getBasePath() + "/images/person.jpg";
    std::string output;

    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ASSERT_TRUE(ai.startup());
    ASSERT_TRUE(ai.isStarted());
    EXPECT_TRUE(ai.pipeCreate(facePipeId, facePipeConfig));
    EXPECT_TRUE(ai.pipeDetectFromFile(facePipeId, inputPath, output));
    EXPECT_TRUE(ai.pipeDelete(facePipeId));
    EXPECT_TRUE(ai.shutdown());

    rj::Document d;
    d.Parse(output.c_str());
    ASSERT_TRUE(d.IsObject());
    ASSERT_TRUE(d.HasMember("returnCode"));
    EXPECT_EQ(d["returnCode"].GetInt(), 0);
    ASSERT_TRUE(d["results"].GetArray()[0][faceNodeId.c_str()].HasMember("faces"));
    EXPECT_EQ(d["results"].GetArray()[0][faceNodeId.c_str()]["faces"].Size(), 1);
}

TEST_F(PipeApiFacadeTest, pipeCreateDeleteTest)
{
    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ASSERT_TRUE(ai.startup());
    ASSERT_TRUE(ai.isStarted());
    EXPECT_TRUE(ai.pipeCreate(facePipeId, facePipeConfig));
    EXPECT_TRUE(ai.pipeDelete(facePipeId));
    EXPECT_TRUE(ai.pipeCreate(facePipeId, facePipeConfig));
    EXPECT_TRUE(ai.pipeDelete(facePipeId));
    EXPECT_TRUE(ai.shutdown());
}

TEST_F(PipeApiFacadeTest, usePipeApiBeforeStartup)
{
    std::string basePath = AIVision::getBasePath();
    cv::Mat input = cv::imread(basePath + "/images/person.jpg", cv::IMREAD_COLOR);
    std::string output;

    EdgeAIVision& ai = EdgeAIVision::getInstance();
    EXPECT_FALSE(ai.isStarted());
    EXPECT_FALSE(ai.pipeCreate(facePipeId, facePipeConfig));
    EXPECT_FALSE(ai.pipeDetect(facePipeId, input, output));
    EXPECT_FALSE(ai.pipeDelete(facePipeId));
    EXPECT_FALSE(ai.shutdown());
}

TEST_F(PipeApiFacadeTest, usePipeApiAfterShutdown)
{
    std::string basePath = AIVision::getBasePath();
    cv::Mat input = cv::imread(basePath + "/images/person.jpg", cv::IMREAD_COLOR);
    std::string output;

    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ASSERT_TRUE(ai.startup());
    ASSERT_TRUE(ai.isStarted());
    EXPECT_TRUE(ai.pipeCreate(facePipeId, facePipeConfig));
    EXPECT_TRUE(ai.pipeDetect(facePipeId, input, output));
    EXPECT_TRUE(ai.pipeDelete(facePipeId));
    EXPECT_TRUE(ai.shutdown());

    EXPECT_FALSE(ai.pipeCreate(facePipeId, facePipeConfig));
    EXPECT_FALSE(ai.pipeDetect(facePipeId, input, output));
    EXPECT_FALSE(ai.pipeDelete(facePipeId));
}
