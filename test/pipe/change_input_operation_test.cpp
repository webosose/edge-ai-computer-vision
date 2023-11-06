/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/PipeNode.h>
#include <aif/base/AIVision.h>
#include <aif/pipe/ChangeInputOperation.h>
#include <aif/tools/Utils.h>
#include <gtest/gtest.h>
#include "ConfigUtil.h"

#include <opencv2/opencv.hpp>
using namespace std;
using namespace aif;

class ChangeInputOperationTest : public ::testing::Test
{
protected:
    ChangeInputOperationTest() = default;
    ~ChangeInputOperationTest() = default;

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
    std::string id = "change_input_test";
    std::string testConfig = R"(
        {
            "type" : "change_input",
            "config": {
                "changeRect" : [0, 60, 4, 60]
            }
        })";
    std::string testConfigInvalid1 = R"(
        {
            "type" : "change_input",
            "config": {
                "changeRect" : [0, 120, 4, 1]
            }
        })";
    std::string testConfigInvalid2 = R"(
        {
            "type" : "change_input",
            "config": {
                "changeRect" : [0, 60, 5, 10]
            }
        })";
    std::string testConfigInvalid3 = R"(
        {
            "type" : "change_input",
            "config": {
                "changeRect" : [0, 100, 4, 40]
            }
        })";
};

TEST_F(ChangeInputOperationTest, change_input_test)
{
    auto config = std::make_shared<ChangeInputOperationConfig>();
    config->parse(ConfigUtil::stringToJson(testConfig));
    auto noConfig = std::dynamic_pointer_cast<NodeOperationConfig>(config);

    NodeType inputType(NodeType::IMAGE);
    NodeType outputType(NodeType::IMAGE);

    const auto& descriptor = make_shared<PipeDescriptor>();
    const auto& input = make_shared<NodeInput>(id, inputType);
    const auto& output = make_shared<NodeOutput>(id, outputType);

    cv::Mat mat(120, 4, CV_64F);
    for(auto i = 0; i < 120; i++){
        for(auto j = 0; j < 4; j++) {
            mat.at<double>(i, j) = i * 0.1f;
        }
    }

    descriptor->setImage(mat);
    input->setDescriptor(descriptor);

    auto operation = NodeOperationFactory::get().create(id, noConfig);
    EXPECT_TRUE(operation->run(input, output));

    cv::Mat result = output->getDescriptor()->getImage();
    EXPECT_EQ(output->getDescriptor()->getImage().size(), cv::Size(4, 60));
    for(auto i = 0; i < 60; i++) {
        for(auto j = 0; j < 4; j++) {
            EXPECT_TRUE(floatEquals(result.at<double>(i, j), (i + 60) * 0.1f));
        }
    }
}

TEST_F(ChangeInputOperationTest, change_input_invalid1_test)
{
    auto config = std::make_shared<ChangeInputOperationConfig>();
    config->parse(ConfigUtil::stringToJson(testConfigInvalid1));
    auto noConfig = std::dynamic_pointer_cast<NodeOperationConfig>(config);

    NodeType inputType(NodeType::IMAGE);
    NodeType outputType(NodeType::IMAGE);

    const auto& descriptor = make_shared<PipeDescriptor>();
    const auto& input = make_shared<NodeInput>(id, inputType);
    const auto& output = make_shared<NodeOutput>(id, outputType);

    cv::Mat mat(120, 4, CV_64F);
    descriptor->setImage(mat);
    input->setDescriptor(descriptor);

    auto operation = NodeOperationFactory::get().create(id, noConfig);
    EXPECT_FALSE(operation->run(input, output));
}

TEST_F(ChangeInputOperationTest, change_input_invalid2_test)
{
    auto config = std::make_shared<ChangeInputOperationConfig>();
    config->parse(ConfigUtil::stringToJson(testConfigInvalid2));
    auto noConfig = std::dynamic_pointer_cast<NodeOperationConfig>(config);

    NodeType inputType(NodeType::IMAGE);
    NodeType outputType(NodeType::IMAGE);

    const auto& descriptor = make_shared<PipeDescriptor>();
    const auto& input = make_shared<NodeInput>(id, inputType);
    const auto& output = make_shared<NodeOutput>(id, outputType);

    cv::Mat mat(120, 4, CV_64F);
    descriptor->setImage(mat);
    input->setDescriptor(descriptor);

    auto operation = NodeOperationFactory::get().create(id, noConfig);
    EXPECT_FALSE(operation->run(input, output));
}

TEST_F(ChangeInputOperationTest, change_input_invalid3_test)
{
    auto config = std::make_shared<ChangeInputOperationConfig>();
    config->parse(ConfigUtil::stringToJson(testConfigInvalid3));
    auto noConfig = std::dynamic_pointer_cast<NodeOperationConfig>(config);

    NodeType inputType(NodeType::IMAGE);
    NodeType outputType(NodeType::IMAGE);

    const auto& descriptor = make_shared<PipeDescriptor>();
    const auto& input = make_shared<NodeInput>(id, inputType);
    const auto& output = make_shared<NodeOutput>(id, outputType);

    cv::Mat mat(120, 4, CV_64F);
    descriptor->setImage(mat);
    input->setDescriptor(descriptor);

    auto operation = NodeOperationFactory::get().create(id, noConfig);
    EXPECT_FALSE(operation->run(input, output));
}
