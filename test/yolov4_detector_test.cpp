/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/Detector.h>
#include <aif/base/Descriptor.h>
#include <aif/bodyPoseEstimation/personDetect/yolov4/Yolov4Descriptor.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <vector>

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
        "    }"
        "  ]"
        "}"};


    std::string use_npu_delegate_and_people {
        "{"
        "  \"delegates\" : ["
        "    {"
        "      \"name\": \"npu_delegate\","
        "      \"option\": {"
        "       }"
        "    }"
        "  ],"
        "  \"commonParam\" : {"
        "    \"numMaxPerson\": 5"
        "  }"
        "}"};




};

TEST_F(Yolov4DetectorTest, 01_yolov4_detect_person)
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
    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/yoga.png", descriptor) == aif::kAifOk);
    foundYolov4s->drawBbox(basePath + "/images/yoga.png");

    foundYolov4s->clear();
    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/test_arms.png", descriptor) == aif::kAifOk);
    foundYolov4s->drawBbox(basePath + "/images/test_arms.png");

    std::cout << foundYolov4s->toStr() << std::endl;
    EXPECT_EQ(foundYolov4s->size(), 1);
}

TEST_F(Yolov4DetectorTest, 02_yolov4_detect_people)
{
    auto fd = DetectorFactory::get().getDetector("person_yolov4_npu", use_npu_delegate_and_people);
    EXPECT_TRUE(fd.get() != nullptr);
    EXPECT_EQ(fd->getModelName(), "FitTV_Detector.tflite");
    auto modelInfo = fd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 416);
    EXPECT_EQ(modelInfo.width, 416);
    EXPECT_EQ(modelInfo.channels, 3);

    std::shared_ptr<Descriptor> descriptor = std::make_shared<Yolov4Descriptor>();
    auto foundYolov4s = std::dynamic_pointer_cast<Yolov4Descriptor>(descriptor);

    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/people.jpg", descriptor) == aif::kAifOk);
    foundYolov4s->drawBbox(basePath + "/images/people.jpg");
    std::cout << foundYolov4s->toStr() << std::endl;
    EXPECT_EQ(foundYolov4s->size(), 5);


    foundYolov4s->clear();
    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/FitTV_sample_000000.jpg", descriptor) == aif::kAifOk);
    foundYolov4s->drawBbox(basePath + "/images/FitTV_sample_000000.jpg");
    std::cout << foundYolov4s->toStr() << std::endl;
    EXPECT_EQ(foundYolov4s->size(), 1);
}

TEST_F(Yolov4DetectorTest, 03_yolov4_detect_side_person)
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


    for (auto i = 1; i < 10; i++) {
        std::string inputPathDir = R"(/usr/share/aif/images/pose_model_val_data/)";
        inputPathDir += std::to_string(i);
        std::vector<cv::String> files;
        cv::glob( inputPathDir, files );

        for (int j = 0; j < files.size(); j++) {
            foundYolov4s->clear();
            EXPECT_TRUE(fd->detectFromImage(files[j], descriptor) == aif::kAifOk);
            foundYolov4s->drawBbox(files[j]);
            std::cout << "files[j] " << files[j] << " drawBbox done!\n";
        }
    }
    //std::cout << foundYolov4s->toStr() << std::endl;
    //EXPECT_EQ(foundYolov4s->size(), 1);
}
