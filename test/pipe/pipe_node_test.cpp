/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/PipeNode.h>
#include <aif/base/AIVision.h>
#include <gtest/gtest.h>
#include "ConfigUtil.h"

#include <opencv2/opencv.hpp>
using namespace std;
using namespace aif;

class PipeNodeTest : public ::testing::Test
{
protected:
    PipeNodeTest() = default;
    ~PipeNodeTest() = default;

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
    std::string detectorNodeCfg= R"(
        {
            "id" : "detect_face",
            "input" : ["image"],
            "output" : ["image", "inference"],
            "operation" : {
                "type" : "detector",
                "config": {
                    "model": "face_yunet_cpu",
                    "param": {
                        "autoDelegate": {
                            "policy": "CPU_ONLY"
                        }
                    }
                }
            }
        })";

    std::string bridgeNodeCfg = R"(
        {
            "id" : "palm_crop",
            "input" : ["image", "inference"],
            "output" : ["image", "inference"],
            "operation" : {
                "type" : "palm_crop",
                "config": {
                    "targetId" : "detect_face"
                }
            }
        })";

    std::string testNodeCfg1= R"(
        {
            "id" : "palm_crop",
            "input" : ["image"],
            "output" : ["image", "inference"],
            "operation" : {
                "type" : "palm_crop",
                "config": {
                    "targetId" : "detect_face"
                }
            }
        })";

    std::string testNodeCfg2= R"(
        {
            "id" : "palm_crop",
            "input" : ["image", "inference"],
            "output" : ["inference"],
            "operation" : {
                "type" : "palm_crop",
                "config": {
                    "targetId" : "detect_face"
                }
            }
        })";

 };

TEST_F(PipeNodeTest, 01_build_detector_node)
{
    auto config = ConfigUtil::getConfig<NodeConfig>(detectorNodeCfg);
    PipeNode node;
    EXPECT_TRUE(node.build(config));
    EXPECT_EQ(node.getId(), "detect_face");
    EXPECT_EQ(node.getInput()->getType(), NodeType(NodeType::IMAGE));
    EXPECT_EQ(node.getOutput()->getType(), NodeType(NodeType::IMAGE|NodeType::INFERENCE));
}

TEST_F(PipeNodeTest, 02_build_bridge_node)
{
    auto config = ConfigUtil::getConfig<NodeConfig>(bridgeNodeCfg);
    PipeNode node;
    EXPECT_TRUE(node.build(config));
    EXPECT_EQ(node.getId(), "palm_crop");
    EXPECT_EQ(node.getInput()->getType(), NodeType(NodeType::IMAGE|NodeType::INFERENCE));
    EXPECT_EQ(node.getOutput()->getType(), NodeType(NodeType::IMAGE|NodeType::INFERENCE));
}

TEST_F(PipeNodeTest, 03_setDescriptor)
{
    auto config = ConfigUtil::getConfig<NodeConfig>(detectorNodeCfg);
    PipeNode node;
    EXPECT_TRUE(node.build(config));

    auto descriptor = std::make_shared<NodeDescriptor>();
    EXPECT_FALSE(node.setDescriptor(descriptor));

    descriptor->addOperationResult("detect_face", R"("{"item1":"test"})");
    EXPECT_FALSE(node.setDescriptor(descriptor));

    cv::Mat image = cv::imread(basePath + "/images/person.jpg");
    descriptor->setImage(image);
    EXPECT_TRUE(node.setDescriptor(descriptor));
    EXPECT_EQ(descriptor.get(), node.getInput()->getDescriptor().get());
}

TEST_F(PipeNodeTest, 04_moveDescriptor)
{
    auto config1 = ConfigUtil::getConfig<NodeConfig>(detectorNodeCfg);
    auto node1 = std::make_shared<PipeNode>();
    EXPECT_TRUE(node1->build(config1));

    auto config2 = ConfigUtil::getConfig<NodeConfig>(bridgeNodeCfg);
    auto node2 = std::make_shared<PipeNode>();
    EXPECT_TRUE(node2->build(config2));

    auto descriptor = std::make_shared<NodeDescriptor>();
    cv::Mat image = cv::imread(basePath + "/images/person.jpg");
    descriptor->setImage(image);
    EXPECT_TRUE(node1->setDescriptor(descriptor));
    EXPECT_FALSE(node1->moveDescriptor(node2));

    descriptor->addOperationResult("detect_face", R"("{"item1":"test"})");
    EXPECT_TRUE(node1->getOutput()->setDescriptor(descriptor));
    EXPECT_TRUE(node1->moveDescriptor(node2));

    EXPECT_EQ(descriptor.get(), node1->getInput()->getDescriptor().get());
    EXPECT_EQ(descriptor.get(), node1->getOutput()->getDescriptor().get());
    EXPECT_EQ(descriptor.get(), node2->getInput()->getDescriptor().get());
}

TEST_F(PipeNodeTest, 05_verifyConnect)
{
    auto config1 = ConfigUtil::getConfig<NodeConfig>(testNodeCfg1);
    auto node1 = std::make_shared<PipeNode>();
    EXPECT_TRUE(node1->build(config1));

    auto config2 = ConfigUtil::getConfig<NodeConfig>(testNodeCfg2);
    auto node2 = std::make_shared<PipeNode>();
    EXPECT_TRUE(node2->build(config2));

    EXPECT_FALSE(node1->verifyConnectPrev(node2));
    EXPECT_TRUE(node1->verifyConnectNext(node2));

    EXPECT_TRUE(node2->verifyConnectPrev(node1));
    EXPECT_FALSE(node2->verifyConnectNext(node1));
}
