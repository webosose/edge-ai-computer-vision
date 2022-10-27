/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/Detector.h>
#include <aif/base/Descriptor.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3/Yolov3Descriptor.h>
#include <aif/bodyPoseEstimation/personDetect/yolov3/Yolov3Detector.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

#include <cstdlib>

using namespace aif;

class Yolov3DetectorTest : public ::testing::Test
{
protected:
    Yolov3DetectorTest() = default;
    ~Yolov3DetectorTest() = default;

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


    std::string use_npu_delegate_and_face_detect {
        "{"
        "  \"delegates\" : ["
        "    {"
        "      \"name\": \"npu_delegate\","
        "      \"option\": {"
        "       }"
        "    }"
        "  ],"
        "  \"modelParam\" : {"
        "    \"detectObject\": \"face\","
        "    \"thresh_score\" : ["
        "                       0,"
        "                       24000,"     // make higher : low score bbox not detected.
        "                       35000,"
        "                       58981"
        "    ],"
        "    \"thresh_iou_sc_nms\" : 115,"  // make higher : duplicated bboxes can be detected.
        "    \"thresh_iou_sc_sur\" : 200"
        "  }"
        "}"};

};


TEST_F(Yolov3DetectorTest, 01_yolov3_detect_person)
{
    auto fd = std::dynamic_pointer_cast<Yolov3Detector>(DetectorFactory::get().getDetector("person_yolov3_npu", use_npu_delegate));
    EXPECT_TRUE(fd.get() != nullptr);
    EXPECT_EQ(fd->getModelName(), "FitTV_Detector_Yolov3.tflite");
    auto modelInfo = fd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 270);
    EXPECT_EQ(modelInfo.width, 480);
    EXPECT_EQ(modelInfo.channels, 3);

    std::shared_ptr<Descriptor> descriptor = std::make_shared<Yolov3Descriptor>();
    auto foundYolov3s = std::dynamic_pointer_cast<Yolov3Descriptor>(descriptor);

    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    foundYolov3s->drawBbox(basePath + "/images/person.jpg");
    std::cout << foundYolov3s->toStr() << std::endl;
    EXPECT_EQ(foundYolov3s->size(), 1);
    EXPECT_EQ(foundYolov3s->sizeOfFaces(), 0);

    foundYolov3s->clear();
    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/FitTV_sample_000000.jpg", descriptor) == aif::kAifOk);
    foundYolov3s->drawBbox(basePath + "/images/FitTV_sample_000000.jpg");
    std::cout << foundYolov3s->toStr() << std::endl;
    EXPECT_EQ(foundYolov3s->size(), 1);
    EXPECT_EQ(foundYolov3s->sizeOfFaces(), 0);


    foundYolov3s->clear();
    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/test_arms.png", descriptor) == aif::kAifOk);
    foundYolov3s->drawBbox(basePath + "/images/test_arms.png");
    std::cout << foundYolov3s->toStr() << std::endl;
    EXPECT_EQ(foundYolov3s->size(), 1);
    EXPECT_EQ(foundYolov3s->sizeOfFaces(), 0);


    foundYolov3s->clear();
    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/yoga.png", descriptor) == aif::kAifOk);
    foundYolov3s->drawBbox(basePath + "/images/yoga.png");
    std::cout << foundYolov3s->toStr() << std::endl;
    EXPECT_EQ(foundYolov3s->size(), 1);
    EXPECT_EQ(foundYolov3s->sizeOfFaces(), 0);

}

TEST_F(Yolov3DetectorTest, 02_yolov3_detect_face)
{
    auto fd = std::dynamic_pointer_cast<Yolov3Detector>(DetectorFactory::get().getDetector("person_yolov3_npu", use_npu_delegate_and_face_detect));
    EXPECT_TRUE(fd.get() != nullptr);
    EXPECT_EQ(fd->getModelName(), "FitTV_Detector_Yolov3.tflite");
    auto modelInfo = fd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 270);
    EXPECT_EQ(modelInfo.width, 480);
    EXPECT_EQ(modelInfo.channels, 3);

    std::shared_ptr<Descriptor> descriptor = std::make_shared<Yolov3Descriptor>();
    auto foundYolov3s = std::dynamic_pointer_cast<Yolov3Descriptor>(descriptor);

    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    foundYolov3s->drawBbox(basePath + "/images/person.jpg");
    std::cout << foundYolov3s->toStr() << std::endl;
    EXPECT_EQ(foundYolov3s->sizeOfFaces(), 1);
    EXPECT_EQ(foundYolov3s->size(), 0);


    foundYolov3s->clear();
    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/FitTV_sample_000000.jpg", descriptor) == aif::kAifOk);
    foundYolov3s->drawBbox(basePath + "/images/FitTV_sample_000000.jpg");
    std::cout << foundYolov3s->toStr() << std::endl;
    EXPECT_EQ(foundYolov3s->sizeOfFaces(), 0); /* far away, mask man */
    EXPECT_EQ(foundYolov3s->size(), 0);


    foundYolov3s->clear();
    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/test_arms.png", descriptor) == aif::kAifOk);
    foundYolov3s->drawBbox(basePath + "/images/test_arms.png");
    std::cout << foundYolov3s->toStr() << std::endl;
    EXPECT_EQ(foundYolov3s->sizeOfFaces(), 0); /* not detected */
    EXPECT_EQ(foundYolov3s->size(), 0);


    foundYolov3s->clear();
    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/yoga.png", descriptor) == aif::kAifOk);
    foundYolov3s->drawBbox(basePath + "/images/yoga.png");
    std::cout << foundYolov3s->toStr() << std::endl;
    EXPECT_EQ(foundYolov3s->sizeOfFaces(), 0); /* not detected */
    EXPECT_EQ(foundYolov3s->size(), 0);
}

TEST_F(Yolov3DetectorTest, 03_yolov3_detect_90_pose_person)
{
    auto fd = DetectorFactory::get().getDetector("person_yolov3_npu", use_npu_delegate);
    //auto fd = DetectorFactory::get().getDetector("person_yolov3_npu", use_npu_delegate_and_face_detect);

    EXPECT_TRUE(fd.get() != nullptr);
    EXPECT_EQ(fd->getModelName(), "FitTV_Detector_Yolov3.tflite");
    auto modelInfo = fd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 270);
    EXPECT_EQ(modelInfo.width, 480);
    EXPECT_EQ(modelInfo.channels, 3);

    std::shared_ptr<Descriptor> descriptor = std::make_shared<Yolov3Descriptor>();
    auto foundYolov3s = std::dynamic_pointer_cast<Yolov3Descriptor>(descriptor);


    for (auto i = 1; i < 10; i++) {
        std::string inputPathDir = R"(/usr/share/aif/images/pose_model_val_data/)";
        inputPathDir += std::to_string(i);
        std::vector<cv::String> files;
        cv::glob( inputPathDir, files );

        for (int j = 0; j < files.size(); j++) {
            foundYolov3s->clear();
            EXPECT_TRUE(fd->detectFromImage(files[j], descriptor) == aif::kAifOk);
            foundYolov3s->drawBbox(files[j]);
            std::cout << "files[j] " << files[j] << " drawBbox done!\n";
        }
    }
    //std::cout << foundYolov3s->toStr() << std::endl;
    //EXPECT_EQ(foundYolov3s->size(), 1);
}
