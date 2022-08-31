/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/PipeConfig.h>
#include <gtest/gtest.h>
#include "ConfigUtil.h"

using namespace std;
using namespace aif;

class PipeConfigTest: public ::testing::Test
{
protected:
    PipeConfigTest() = default;
    ~PipeConfigTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

public:
    std::string bridgeOpCfg = R"(
        {
            "type" : "palm_crop",
            "config" : {
                "targetId" : "palm"
            }
        })";

    std::string bridgeOpCfg_no_targetId = R"(
        {
            "type" : "palm_crop",
            "config" : {}
        })";

    std::string detectorOpCfg = R"(
        {
            "type" : "detector",
            "config": {
                "model": "palm_lite_cpu",
                "param": {
                    "autoDelegate": {
                        "policy": "CPU_ONLY"
                     }
                }
            }
        })";

    std::string detectorOpCfg_no_type = R"(
        {
            "config": {
                "model": "palm_lite_cpu",
                "param": {
                    "autoDelegate": {
                        "policy": "CPU_ONLY"
                     }
                }
            }
        })";


    std::string detectorOpCfg_no_config = R"(
        {
            "type" : "detector"
        })";


     std::string detectorOpCfg_no_config_model = R"(
        {
            "type" : "detector",
            "config": {
                "param": {
                    "autoDelegate": {
                        "policy": "CPU_ONLY"
                     }
                }
            }
        })";
     std::string detectorOpCfg_no_config_param = R"(
        {
            "type" : "detector",
            "config": {
                "model": "palm_lite_cpu"
            }
        })";

    std::string nodeCfg = R"(
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
        })";
    std::string nodeCfg_no_id = R"(
        {
            "input" : ["image"],
            "output" : ["image", "inference"],
            "operation" : {
                "type" : "detector",
                "config": {
                    "model": "face_yunet_cpu"
                }
            }
        })";
    std::string nodeCfg_no_input= R"(
        {
            "id" : "detect_face",
            "input" : ["image"],
            "operation" : {
                "type" : "detector",
                "config": {
                    "model": "face_yunet_cpu"
                }
            }
        })";
    std::string nodeCfg_no_output = R"(
        {
            "id" : "detect_face",
            "input" : ["image"],
            "operation" : {
                "type" : "detector",
                "config": {
                    "model": "face_yunet_cpu"
                }
            }
        })";
     std::string nodeCfg_no_operation = R"(
        {
            "id" : "detect_face",
            "input" : ["image"],
            "output" : ["image", "inference"]
        })";
     std::string pipeCfg = R"(
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
    std::string pipeCfg_no_name = R"(
        {
            "nodes": [
                {
                    "id" : "detect_palm",
                    "input" : ["image"],
                    "output" : ["image", "inference"],
                    "operation" : {
                        "type" : "detector",
                        "config": {
                            "model": "palm_lite_cpu"
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
                            "model": "handlandmark_lite_cpu"
                        }
                    }
                }
            ]
        }
    )";
    std::string pipeCfg_no_nodes = R"(
        {
            "name" : "pipe_hand_gesture"
        }
    )";
    std::string pipeCfg_no_nodes2 = R"(
        {
            "name" : "pipe_hand_gesture",
            "nodes" : []
        }
    )";
};


TEST_F(PipeConfigTest, NodeOperationConfig)
{
    NodeOperationConfig config;
    EXPECT_TRUE(config.parse(ConfigUtil::stringToJson(bridgeOpCfg)));
    EXPECT_EQ(config.getType(), "palm_crop");
    EXPECT_TRUE(!config.getConfig().empty());

    NodeOperationConfig config2;
    EXPECT_TRUE(config2.parse(ConfigUtil::stringToJson(detectorOpCfg)));
    EXPECT_EQ(config2.getType(), "detector");
    EXPECT_TRUE(!config2.getConfig().empty());
}

TEST_F(PipeConfigTest, NodeOperationConfig_no_type)
{
    NodeOperationConfig config;
    EXPECT_FALSE(config.parse(ConfigUtil::stringToJson(detectorOpCfg_no_type)));
}

TEST_F(PipeConfigTest, NodeOperationConfig_no_config)
{
    NodeOperationConfig config;
    EXPECT_FALSE(config.parse(ConfigUtil::stringToJson(detectorOpCfg_no_config)));
}

TEST_F(PipeConfigTest, BridgeOperationConfig)
{
    BridgeOperationConfig config;
    EXPECT_TRUE(config.parse(ConfigUtil::stringToJson(bridgeOpCfg)));
    EXPECT_EQ(config.getType(), "palm_crop");
    EXPECT_EQ(config.getTargetId(), "palm");
}

TEST_F(PipeConfigTest, DetectorOperationConfig)
{
    DetectorOperationConfig config;
    EXPECT_TRUE(config.parse(ConfigUtil::stringToJson(detectorOpCfg)));
    EXPECT_EQ(config.getType(), "detector");
    EXPECT_EQ(config.getModel(), "palm_lite_cpu");
    EXPECT_TRUE(!config.getParam().empty());
}

TEST_F(PipeConfigTest, DetectorOperationConfig_no_type)
{
    DetectorOperationConfig config;
    EXPECT_FALSE(config.parse(ConfigUtil::stringToJson(detectorOpCfg_no_type)));
}

TEST_F(PipeConfigTest, DetectorOperationConfig_no_config)
{
    DetectorOperationConfig config;
    EXPECT_FALSE(config.parse(ConfigUtil::stringToJson(detectorOpCfg_no_config)));
}

TEST_F(PipeConfigTest, DetectorOperationConfig_no_config_model)
{
    DetectorOperationConfig config;
    EXPECT_FALSE(config.parse(ConfigUtil::stringToJson(detectorOpCfg_no_config_model)));
}

TEST_F(PipeConfigTest, DetectorOperationConfig_no_config_param)
{
    DetectorOperationConfig config;
    EXPECT_TRUE(config.parse(ConfigUtil::stringToJson(detectorOpCfg_no_config_param)));
    EXPECT_EQ(config.getType(), "detector");
    EXPECT_EQ(config.getModel(), "palm_lite_cpu");
    EXPECT_TRUE(config.getParam().empty());
}

TEST_F(PipeConfigTest, NodeConfig)
{
    NodeConfig config;
    EXPECT_TRUE(config.parse(ConfigUtil::stringToJson(nodeCfg)));
    EXPECT_EQ(config.getId(), "detect_face");
    EXPECT_EQ(config.getInputType(), NodeType(NodeType::IMAGE));
    EXPECT_EQ(config.getOutputType(),
            NodeType(NodeType::IMAGE|NodeType::INFERENCE));
    EXPECT_TRUE(config.getOperation() !=  nullptr);
}

TEST_F(PipeConfigTest, NodeConfig_invalid)
{
    NodeConfig config;
    EXPECT_FALSE(config.parse(ConfigUtil::stringToJson(nodeCfg_no_id)));
    EXPECT_FALSE(config.parse(ConfigUtil::stringToJson(nodeCfg_no_input)));
    EXPECT_FALSE(config.parse(ConfigUtil::stringToJson(nodeCfg_no_output)));
    EXPECT_FALSE(config.parse(ConfigUtil::stringToJson(nodeCfg_no_operation)));
}

TEST_F(PipeConfigTest, PipeConfig)
{
    PipeConfig config;
    EXPECT_TRUE(config.parse(pipeCfg));
    EXPECT_EQ(config.getName(), "pipe_hand_gesture");
    EXPECT_EQ(config.getNodeSize(), 3);
    EXPECT_TRUE(config.getNode(0) != nullptr);
    EXPECT_EQ(config.getNode(0)->getId(), "detect_palm");
    EXPECT_TRUE(config.getNode(1) != nullptr);
    EXPECT_EQ(config.getNode(1)->getId(), "palm_crop");
    EXPECT_TRUE(config.getNode(2) != nullptr);
    EXPECT_EQ(config.getNode(2)->getId(), "detect_hand_landmark");
    EXPECT_TRUE(config.getNode(3) == nullptr);
}

TEST_F(PipeConfigTest, PipeConfig_invalid)
{
    PipeConfig config;
    EXPECT_FALSE(config.parse(pipeCfg_no_name));
    EXPECT_FALSE(config.parse(pipeCfg_no_nodes));
    EXPECT_FALSE(config.parse(pipeCfg_no_nodes2));
}
