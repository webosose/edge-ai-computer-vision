/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/semantic/SemanticDescriptor.h>

#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>

#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;

class SemanticDetectorTest : public ::testing::Test
{
protected:
    SemanticDetectorTest() = default;
    ~SemanticDetectorTest() = default;

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
        DetectorFactory::get().clear();
        basePath = AIVision::getBasePath();
    }

    void TearDown() override
    {
    }

    std::string basePath;
};


TEST_F(SemanticDetectorTest, 01_getModelInfo)
{
    auto dt = DetectorFactory::get().getDetector("semantic_deeplabv3_cpu");
    ASSERT_TRUE(dt.get() != nullptr);
    ASSERT_EQ(dt->getModelName(),
            "deeplabv3_mnv2_dm05_pascal_quant.tflite");
    auto modelInfo = dt->getModelInfo();
    EXPECT_EQ(modelInfo.height, 513);
    EXPECT_EQ(modelInfo.width, 513);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(SemanticDetectorTest, 02_detect)
{
    auto dt = DetectorFactory::get().getDetector("semantic_deeplabv3_cpu");
    ASSERT_TRUE(dt.get() != nullptr);

    SemanticDescriptor* semanticDescriptor = new SemanticDescriptor();
    std::shared_ptr<Descriptor> descriptor(semanticDescriptor);
    EXPECT_FALSE(descriptor->hasMember("segments"));
    ASSERT_TRUE(dt->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_TRUE(descriptor->hasMember("segments"));
}


TEST_F(SemanticDetectorTest, 03_detect_base64)
{
    auto dt = DetectorFactory::get().getDetector("semantic_deeplabv3_cpu");
    ASSERT_TRUE(dt.get() != nullptr);

    auto base64image = aif::fileToStr(basePath + "/images/mona_base64.jpg"); // 128 x 128
    SemanticDescriptor* semanticDescriptor = new SemanticDescriptor();
    std::shared_ptr<Descriptor> descriptor(semanticDescriptor);
    EXPECT_FALSE(descriptor->hasMember("segments"));
    ASSERT_TRUE(dt->detectFromBase64(base64image, descriptor) == aif::kAifOk);
    EXPECT_TRUE(descriptor->hasMember("segments"));
}

#ifdef USE_EDGETPU
TEST_F(SemanticDetectorTest, 04_edgetpu_detect)
{
    auto dt = DetectorFactory::get().getDetector("semantic_deeplabv3_edgetpu");
    ASSERT_TRUE(dt.get() != nullptr);

    SemanticDescriptor* semanticDescriptor = new SemanticDescriptor();
    std::shared_ptr<Descriptor> descriptor(semanticDescriptor);
    EXPECT_FALSE(descriptor->hasMember("segments"));
    ASSERT_TRUE(dt->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_TRUE(descriptor->hasMember("segments"));
}
#endif


