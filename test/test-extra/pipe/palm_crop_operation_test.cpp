/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/pipe/PalmCropOperation.h>
#include <aifex/palm/PalmDescriptor.h>

#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>
#include "pipe/ConfigUtil.h"

using namespace std;
using namespace aif;

class PalmCropOperationTest: public ::testing::Test
{
protected:
    PalmCropOperationTest() = default;
    ~PalmCropOperationTest() = default;

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
    std::string cropConfig = R"(
        {
            "type" : "palm_crop",
            "config" : {
                "targetId" : "detect_palm"
            }
        })";
};

TEST_F(PalmCropOperationTest, 01_run)
{
    cv::Mat image = cv::imread(basePath + "/images/hand_right.jpg");
    auto detector = DetectorFactory::get().getDetector("palm_lite_cpu");
    ASSERT_TRUE(detector.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<PalmDescriptor>();
    ASSERT_TRUE(detector->detect(image, descriptor) == aif::kAifOk);
    auto foundPalms = std::dynamic_pointer_cast<PalmDescriptor>(descriptor);
    EXPECT_EQ(foundPalms->size(), 1);
    string targetId = "detect_palm";
    string result = foundPalms->toStr();

    const auto& pipeDescriptor = make_shared<PipeDescriptor>();
    pipeDescriptor->setImage(image);
    pipeDescriptor->addDetectorOperationResult(targetId, "test_model", descriptor);

    string id = "palm_crop";
    std::shared_ptr<NodeOperationConfig> config =
        ConfigUtil::getOperationConfig<BridgeOperationConfig>(cropConfig);
    NodeType inputType(NodeType::IMAGE);
    NodeType outputType(NodeType::IMAGE|NodeType::INFERENCE);
    const auto& input = make_shared<NodeInput>(id, inputType);
    const auto& output = make_shared<NodeOutput>(id, outputType);
    const auto& operation = NodeOperationFactory::get().create(id, config);

    input->setDescriptor(pipeDescriptor);
    cv::Size originSize = input->getDescriptor()->getImage().size();
    ASSERT_TRUE(operation->run(input, output));
    cv::Size cropSize = output->getDescriptor()->getImage().size();
    EXPECT_NE(originSize, cropSize);

    string cropResult = output->getDescriptor()->getResult(id);
    rj::Document doc;
    doc.Parse(cropResult);
    ASSERT_TRUE(doc.HasMember("region"));
    EXPECT_EQ(doc["region"][2].GetInt(), cropSize.width);
    EXPECT_EQ(doc["region"][3].GetInt(), cropSize.height);
}
