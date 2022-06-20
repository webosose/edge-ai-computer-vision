/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/selfie/SelfieDescriptor.h>

#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;

class SelfieDetectorTest : public ::testing::Test
{
protected:
    SelfieDetectorTest() = default;
    ~SelfieDetectorTest() = default;

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


TEST_F(SelfieDetectorTest, 01_getModelInfo)
{
    auto dt = DetectorFactory::get().getDetector("selfie_mediapipe_cpu");
    EXPECT_TRUE(dt.get() != nullptr);
    EXPECT_EQ(dt->getModelName(),
            "selfie_segmentation.tflite");
    auto modelInfo = dt->getModelInfo();
    EXPECT_EQ(modelInfo.height, 256);
    EXPECT_EQ(modelInfo.width, 256);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(SelfieDetectorTest, 02_detect)
{
    auto dt = DetectorFactory::get().getDetector("selfie_mediapipe_cpu");
    EXPECT_TRUE(dt.get() != nullptr);

    SelfieDescriptor* selfieDescriptor = new SelfieDescriptor();
    std::shared_ptr<Descriptor> descriptor(selfieDescriptor);
    EXPECT_FALSE(descriptor->hasMember("segments"));
    EXPECT_TRUE(dt->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_TRUE(descriptor->hasMember("segments"));
}


TEST_F(SelfieDetectorTest, 03_detect_base64)
{
    auto dt = DetectorFactory::get().getDetector("selfie_mediapipe_cpu");
    EXPECT_TRUE(dt.get() != nullptr);

    auto base64image = aif::fileToStr(basePath + "/images/mona_base64.txt"); // 128 x 128
    SelfieDescriptor* selfieDescriptor = new SelfieDescriptor();
    std::shared_ptr<Descriptor> descriptor(selfieDescriptor);
    EXPECT_FALSE(descriptor->hasMember("segments"));
    EXPECT_TRUE(dt->detectFromBase64(base64image, descriptor) == aif::kAifOk);
    EXPECT_TRUE(descriptor->hasMember("segments"));
}
