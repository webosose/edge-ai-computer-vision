/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/text/TextDescriptor.h>

#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/Detector.h>
#include <aif/base/Descriptor.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;

class TextDetectorTest : public ::testing::Test
{
protected:
    TextDetectorTest() = default;
    ~TextDetectorTest() = default;

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

TEST_F(TextDetectorTest, 01_init_paddleocr_320_v2)
{
    auto fd = DetectorFactory::get().getDetector("text_paddleocr_320_v2");
    ASSERT_TRUE(fd.get() != nullptr);
    ASSERT_EQ(fd->getModelName(), "paddleocr_320_320_float32.tflite");
    auto modelInfo = fd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 320);
    EXPECT_EQ(modelInfo.width, 320);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(TextDetectorTest, 02_detect_texts)
{
    auto fd = DetectorFactory::get().getDetector("text_paddleocr_320_v2");
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<TextDescriptor>();
    auto foundTexts = std::dynamic_pointer_cast<TextDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/text.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundTexts->getTextRectCount(), 19);
    Logi("Output: ", foundTexts->toStr());
}

TEST_F(TextDetectorTest, 03_detect_texts_with_detection_region)
{
    std::string param = R"(
            {
                "modelParam": {
                    "detectionRegion" : [500, 300, 780, 400]
                }
            })";

    auto fd = DetectorFactory::get().getDetector("text_paddleocr_320_v2", param);
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<TextDescriptor>();
    auto foundTexts = std::dynamic_pointer_cast<TextDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/text.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundTexts->getTextRectCount(), 6);
    Logi("Output: ", foundTexts->toStr());
}
