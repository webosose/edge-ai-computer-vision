/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/Detector.h>
#include <aif/base/Descriptor.h>
#include <aif/bodyPoseEstimation/yolov4/Yolov4Descriptor.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;

class Yolov4DetectorTest : public ::testing::Test
{
protected:
    Yolov4DetectorTest() = default;
    ~Yolov4DetectorTest() = default;

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
    /*std::string use_npu_delegate {
        "{"
        "  \"delegates\" : ["
        "    {"
        "      \"name\": \"npu_delegate\""
        "    }"
        "  ]"
        "}"
    };*/

    std::string use_npu_delegate {
        "{"
        "  \"delegates\" : ["
        "    {"
        "      \"name\": \"npu_delegate\","
        "      \"option\": {"
        "       }"
        "     }"
        "  ]"
        "}"};




};

TEST_F(Yolov4DetectorTest, 01_yolov4_init)
{
    auto fd = DetectorFactory::get().getDetector("person_yolov4_npu", use_npu_delegate);
    EXPECT_TRUE(fd.get() != nullptr);
    EXPECT_EQ(fd->getModelName(), "FitTV_Detector.tflite");
    auto modelInfo = fd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 416);
    EXPECT_EQ(modelInfo.width, 416);
    EXPECT_EQ(modelInfo.channels, 3);

    std::shared_ptr<Descriptor> descriptor = std::make_shared<Yolov4Descriptor>();
    auto foundYolov4s = std::dynamic_pointer_cast<Yolov4Descriptor>(descriptor);
    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/FitTV_sample_000000.jpg", descriptor) == aif::kAifOk);

    std::cout << foundYolov4s->toStr() << std::endl;
    EXPECT_EQ(foundYolov4s->size(), 1);

}
/*
TEST_F(Yolov4DetectorTest, 02_yolov4_from_one_person)
{
    auto fd = DetectorFactory::get().getDetector("person_yolov4_npu", use_npu_delegate);
    EXPECT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<Yolov4Descriptor>();
    auto foundYolov4s = std::dynamic_pointer_cast<Yolov4Descriptor>(descriptor);
    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/FitTV_sample_000000.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundYolov4s->size(), 1);
    std::cout << foundYolov4s->toStr() << std::endl;
}
*/