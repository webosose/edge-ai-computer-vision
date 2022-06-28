/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/bodypix/BodypixDescriptor.h>

#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;

class BodypixDetectorTest : public ::testing::Test
{
protected:
    BodypixDetectorTest() = default;
    ~BodypixDetectorTest() = default;

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
    std::string armnn_delegate_param{
            "{"
            "  \"common\" : {"
            "      \"useXnnpack\": true,"
            "      \"numThreads\": 1"
            "    },"
            "  \"delegates\" : ["
            "    {"
            "      \"name\": \"armnn_delegate\","
            "      \"option\": {"
            "        \"backends\": [\"CpuAcc\"],"
            "        \"logging-severity\": \"info\""
            "      }"
            "    }"
            "  ]"
            "}"
    };

};


TEST_F(BodypixDetectorTest, 01_getModelInfo)
{
    auto dt = DetectorFactory::get().getDetector("bodypix_mobilenet_cpu");
    EXPECT_TRUE(dt.get() != nullptr);
    EXPECT_EQ(dt->getModelName(), "bodypix_mobilenet_v1_075_512_512_16_quant_decoder.tflite");
    auto modelInfo = dt->getModelInfo();
    EXPECT_EQ(modelInfo.height, 512);
    EXPECT_EQ(modelInfo.width, 512);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(BodypixDetectorTest, 02_detect)
{
    auto dt = DetectorFactory::get().getDetector("bodypix_mobilenet_cpu");
    EXPECT_TRUE(dt.get() != nullptr);

    BodypixDescriptor* bodypixDescriptor = new BodypixDescriptor();
    std::shared_ptr<Descriptor> descriptor(bodypixDescriptor);
    EXPECT_FALSE(descriptor->hasMember("segments"));
    EXPECT_TRUE(dt->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_TRUE(descriptor->hasMember("segments"));
}


TEST_F(BodypixDetectorTest, 03_detect_base64)
{
    auto dt = DetectorFactory::get().getDetector("bodypix_mobilenet_cpu");
    EXPECT_TRUE(dt.get() != nullptr);

    auto base64image = aif::fileToStr(basePath + "/images/mona_base64.jpg"); // 128 x 128
    BodypixDescriptor* bodypixDescriptor = new BodypixDescriptor();
    std::shared_ptr<Descriptor> descriptor(bodypixDescriptor);
    EXPECT_FALSE(descriptor->hasMember("segments"));
    EXPECT_TRUE(dt->detectFromBase64(base64image, descriptor) == aif::kAifOk);
    EXPECT_TRUE(descriptor->hasMember("segments"));
}

#ifdef USE_EDGETPU
TEST_F(BodypixDetectorTest, 04_edgetpu_test)
{
    auto dt = DetectorFactory::get().getDetector("bodypix_mobilenet_edgetpu");
    EXPECT_TRUE(dt.get() != nullptr);

    BodypixDescriptor* bodypixDescriptor = new BodypixDescriptor();
    std::shared_ptr<Descriptor> descriptor(bodypixDescriptor);
    EXPECT_FALSE(descriptor->hasMember("segments"));
    EXPECT_TRUE(dt->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_TRUE(descriptor->hasMember("segments"));
}
#endif

#ifdef USE_ARMNN
TEST_F(BodypixDetectorTest, 05_armnn_delegate_test)
{
    auto dt = DetectorFactory::get().getDetector("bodypix_mobilenet_cpu", armnn_delegate_param);
    EXPECT_TRUE(dt.get() != nullptr);
    EXPECT_TRUE(dt->getNumDelegates() == 1);

    BodypixDescriptor* bodypixDescriptor = new BodypixDescriptor();
    std::shared_ptr<Descriptor> descriptor(bodypixDescriptor);
    EXPECT_FALSE(descriptor->hasMember("segments"));
    EXPECT_TRUE(dt->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_TRUE(descriptor->hasMember("segments"));
}

#endif
