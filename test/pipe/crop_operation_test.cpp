/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/CropOperation.h>
#include <aif/base/AIVision.h>
#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>
#include "ConfigUtil.h"

using namespace std;
using namespace aif;

class CropOperationTest: public ::testing::Test
{
protected:
    CropOperationTest() = default;
    ~CropOperationTest() = default;

    static void SetUpTestCase()
    {
        AIVision::init();
    }

    static void TearDownTestCase()
    {
        AIVision::deinit();
    }


    void SetUp() override
    {
        basePath = AIVision::getBasePath();
    }

    void TearDown() override
    {
    }
public:
    std::string basePath;
    static const int cropWidth = 100;
    static const int cropHeight = 100;

    std::string cropConfig = R"(
        {
            "type" : "palm_crop",
            "config" : {
                "targetId" : "palm"
            }
        })";

};

class TestCropOperation : public CropOperation
{
public:
    TestCropOperation(const std::string& id) : CropOperation(id) {}
    cv::Rect getCropRect(const std::shared_ptr<NodeInput>& input) const override
    {
        return cv::Rect(10, 10,
                CropOperationTest::cropWidth,
                CropOperationTest::cropHeight);
    }

};

TEST_F(CropOperationTest, 01_run)
{
    string id = "test_crop";
    std::shared_ptr<NodeOperationConfig> config =
        ConfigUtil::getOperationConfig<BridgeOperationConfig>(cropConfig);

    NodeType inputType(NodeType::IMAGE);
    NodeType outputType(NodeType::IMAGE|NodeType::INFERENCE);

    const auto& descriptor = make_shared<NodeDescriptor>();
    const auto& input = make_shared<NodeInput>(id, inputType);
    const auto& output = make_shared<NodeOutput>(id, outputType);

    cv::Mat image = cv::imread(basePath + "/images/person.jpg");
    descriptor->setImage(image);
    input->setDescriptor(descriptor);

    EXPECT_NE(input->getDescriptor()->getImage().size(), cv::Size(cropWidth, cropHeight));
    TestCropOperation operation("test_crop");
    EXPECT_TRUE(operation.init(config));
    EXPECT_TRUE(operation.run(input, output));
    EXPECT_EQ(output->getDescriptor()->getImage().size(), cv::Size(cropWidth, cropHeight));
}
