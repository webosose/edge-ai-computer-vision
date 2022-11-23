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

    std::string use_npu_delegate_and_not_people_default {
        "{"
        "  \"delegates\" : ["
        "    {"
        "      \"name\": \"npu_delegate\","
        "      \"option\": {"
        "       }"
        "    }"
        "  ]"
        "}"};


    std::string use_npu_delegate_and_not_people {
        "{"
        "  \"delegates\" : ["
        "    {"
        "      \"name\": \"npu_delegate\","
        "      \"option\": {"
        "       }"
        "    }"
        "  ],"
        "  \"commonParam\" : {"
        "    \"numMaxPerson\": 1"
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
    std::string use_npu_delegate_person_in_people {
        "{"
        "  \"delegates\" : ["
        "    {"
        "      \"name\": \"npu_delegate\","
        "      \"option\": {"
        "       }"
        "    }"
        "  ],"
        "  \"commonParam\" : {"
        "    \"numMaxPerson\": 1"
        "  },"
        "  \"modelParam\" : {"
        "    \"origImgRoiX\": 100,"
        "    \"origImgRoiY\": 0,"
        "    \"origImgRoiWidth\": 200,"
        "    \"origImgRoiHeight\": 266"
        "  }"
        "}"};  /* people.jpg is 400 x 266 image.*/
    std::string use_npu_delegate_person2_in_people {
        "{"
        "  \"delegates\" : ["
        "    {"
        "      \"name\": \"npu_delegate\","
        "      \"option\": {"
        "       }"
        "    }"
        "  ],"
        "  \"commonParam\" : {"
        "    \"numMaxPerson\": 2"
        "  },"
        "  \"modelParam\" : {"
        "    \"origImgRoiX\": 100,"
        "    \"origImgRoiY\": 0,"
        "    \"origImgRoiWidth\": 200,"
        "    \"origImgRoiHeight\": 266"
        "  }"
        "}"};  /* people.jpg is 400 x 266 image.*/
    std::string use_npu_delegate_person_in_iou_update {
        "{"
        "  \"delegates\" : ["
        "    {"
        "      \"name\": \"npu_delegate\","
        "      \"option\": {"
        "       }"
        "    }"
        "  ],"
        "  \"modelParam\" : {"
        "    \"thresh_iou_update\": 0.9"
        "  }"
        "}"};

    std::string use_npu_delegate_person_in_iou_update2 {
        "{"
        "  \"delegates\" : ["
        "    {"
        "      \"name\": \"npu_delegate\","
        "      \"option\": {"
        "       }"
        "    }"
        "  ],"
        "  \"modelParam\" : {"
        "    \"thresh_iou_update\": 1.0"
        "  }"
        "}"}; // all updates!

    std::string use_npu_delegate_person_in_iou_update3 {
        "{"
        "  \"delegates\" : ["
        "    {"
        "      \"name\": \"npu_delegate\","
        "      \"option\": {"
        "       }"
        "    }"
        "  ],"
        "  \"modelParam\" : {"
        "    \"thresh_iou_update\": 0.5"
        "  }"
        "}"};  // updates as rarely as possible


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

TEST_F(Yolov3DetectorTest, 04_yolov3_detect_people)
{
    auto fd = DetectorFactory::get().getDetector("person_yolov3_npu", use_npu_delegate_and_people);
    EXPECT_TRUE(fd.get() != nullptr);
    EXPECT_EQ(fd->getModelName(), "FitTV_Detector_Yolov3.tflite");

    std::shared_ptr<Descriptor> descriptor = std::make_shared<Yolov3Descriptor>();
    auto foundYolov3s = std::dynamic_pointer_cast<Yolov3Descriptor>(descriptor);

    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/people.jpg", descriptor) == aif::kAifOk);
    foundYolov3s->drawBbox(basePath + "/images/people.jpg");
    std::cout << foundYolov3s->toStr() << std::endl;
    EXPECT_EQ(foundYolov3s->size(), 5);
}

TEST_F(Yolov3DetectorTest, 05_yolov3_detect_not_people_default)
{
    auto fd = DetectorFactory::get().getDetector("person_yolov3_npu", use_npu_delegate_and_not_people_default);
    EXPECT_TRUE(fd.get() != nullptr);
    EXPECT_EQ(fd->getModelName(), "FitTV_Detector_Yolov3.tflite");

    std::shared_ptr<Descriptor> descriptor = std::make_shared<Yolov3Descriptor>();
    auto foundYolov3s = std::dynamic_pointer_cast<Yolov3Descriptor>(descriptor);

    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/people.jpg", descriptor) == aif::kAifOk);
    foundYolov3s->drawBbox(basePath + "/images/people.jpg");
    std::cout << foundYolov3s->toStr() << std::endl;
    EXPECT_EQ(foundYolov3s->size(), 1);
}
TEST_F(Yolov3DetectorTest, 06_yolov3_detect_not_people)
{
    auto fd = DetectorFactory::get().getDetector("person_yolov3_npu", use_npu_delegate_and_not_people);
    EXPECT_TRUE(fd.get() != nullptr);
    EXPECT_EQ(fd->getModelName(), "FitTV_Detector_Yolov3.tflite");

    std::shared_ptr<Descriptor> descriptor = std::make_shared<Yolov3Descriptor>();
    auto foundYolov3s = std::dynamic_pointer_cast<Yolov3Descriptor>(descriptor);

    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/people.jpg", descriptor) == aif::kAifOk);
    foundYolov3s->drawBbox(basePath + "/images/people.jpg");
    std::cout << foundYolov3s->toStr() << std::endl;
    EXPECT_EQ(foundYolov3s->size(), 1);
}

TEST_F(Yolov3DetectorTest, 07_yolov3_detect_person_in_people)
{
    auto fd = DetectorFactory::get().getDetector("person_yolov3_npu", use_npu_delegate_person_in_people);
    EXPECT_TRUE(fd.get() != nullptr);
    EXPECT_EQ(fd->getModelName(), "FitTV_Detector_Yolov3.tflite");

    std::shared_ptr<Descriptor> descriptor = std::make_shared<Yolov3Descriptor>();
    auto foundYolov3s = std::dynamic_pointer_cast<Yolov3Descriptor>(descriptor);

    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/people.jpg", descriptor) == aif::kAifOk);
    foundYolov3s->drawBbox(basePath + "/images/people.jpg");
    std::cout << foundYolov3s->toStr() << std::endl;
    EXPECT_EQ(foundYolov3s->size(), 1);
}

TEST_F(Yolov3DetectorTest, 08_yolov3_detect_person2_in_people)
{
    auto fd = DetectorFactory::get().getDetector("person_yolov3_npu", use_npu_delegate_person2_in_people);
    EXPECT_TRUE(fd.get() != nullptr);
    EXPECT_EQ(fd->getModelName(), "FitTV_Detector_Yolov3.tflite");

    std::shared_ptr<Descriptor> descriptor = std::make_shared<Yolov3Descriptor>();
    auto foundYolov3s = std::dynamic_pointer_cast<Yolov3Descriptor>(descriptor);

    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/people.jpg", descriptor) == aif::kAifOk);
    foundYolov3s->drawBbox(basePath + "/images/people.jpg");
    std::cout << foundYolov3s->toStr() << std::endl;
    EXPECT_EQ(foundYolov3s->size(), 2);
}

TEST_F(Yolov3DetectorTest, 09_yolov3_detect_person_iou_update)
{
    /* 0.7 */
    auto fd = DetectorFactory::get().getDetector("person_yolov3_npu", use_npu_delegate);
    EXPECT_TRUE(fd.get() != nullptr);
    EXPECT_EQ(fd->getModelName(), "FitTV_Detector_Yolov3.tflite");

    std::shared_ptr<Descriptor> descriptor = std::make_shared<Yolov3Descriptor>();
    auto foundYolov3s = std::dynamic_pointer_cast<Yolov3Descriptor>(descriptor);

    for (auto i = 1; i < 10; i++) {
        std::string inputPathDir = R"(/usr/share/aif/images/pose_model_val_data/)";
        inputPathDir += std::to_string(i);
        std::vector<cv::String> files;
        cv::glob( inputPathDir, files );

        for (int j = 0; j < files.size(); j++) {
            EXPECT_TRUE(fd->detectFromImage(files[j], descriptor) == aif::kAifOk);
        }
        foundYolov3s->drawBbox(files[0]);
        std::cout << "files[j] " << files[0] << " drawBbox done!\n";
    }

    EXPECT_LT(foundYolov3s->size(), 10);
}
TEST_F(Yolov3DetectorTest, 10_yolov3_detect_person_iou_update1)
{
    /* 0.9 */
    auto fd = DetectorFactory::get().getDetector("person_yolov3_npu", use_npu_delegate_person_in_iou_update);
    EXPECT_TRUE(fd.get() != nullptr);
    EXPECT_EQ(fd->getModelName(), "FitTV_Detector_Yolov3.tflite");

    std::shared_ptr<Descriptor> descriptor = std::make_shared<Yolov3Descriptor>();
    auto foundYolov3s = std::dynamic_pointer_cast<Yolov3Descriptor>(descriptor);

    for (auto i = 1; i < 10; i++) {
        std::string inputPathDir = R"(/usr/share/aif/images/pose_model_val_data/)";
        inputPathDir += std::to_string(i);
        std::vector<cv::String> files;
        cv::glob( inputPathDir, files );

        for (int j = 0; j < files.size(); j++) {
            EXPECT_TRUE(fd->detectFromImage(files[j], descriptor) == aif::kAifOk);
        }
        foundYolov3s->drawBbox(files[0]);
        std::cout << "files[j] " << files[0] << " drawBbox done!\n";
    }

    EXPECT_LE(foundYolov3s->size(), 10);
}

TEST_F(Yolov3DetectorTest, 11_yolov3_detect_person_iou_update2)
{
    /* 1.0 */
    auto fd = DetectorFactory::get().getDetector("person_yolov3_npu", use_npu_delegate_person_in_iou_update2);
    EXPECT_TRUE(fd.get() != nullptr);
    EXPECT_EQ(fd->getModelName(), "FitTV_Detector_Yolov3.tflite");

    std::shared_ptr<Descriptor> descriptor = std::make_shared<Yolov3Descriptor>();
    auto foundYolov3s = std::dynamic_pointer_cast<Yolov3Descriptor>(descriptor);

    for (auto i = 1; i < 10; i++) {
        std::string inputPathDir = R"(/usr/share/aif/images/pose_model_val_data/)";
        inputPathDir += std::to_string(i);
        std::vector<cv::String> files;
        cv::glob( inputPathDir, files );

        for (int j = 0; j < files.size(); j++) {
            EXPECT_TRUE(fd->detectFromImage(files[j], descriptor) == aif::kAifOk);
        }
        foundYolov3s->drawBbox(files[0]);
        std::cout << "files[j] " << files[0] << " drawBbox done!\n";
    }

    EXPECT_EQ(foundYolov3s->size(), 10);
}

TEST_F(Yolov3DetectorTest, 12_yolov3_detect_person_iou_update3)
{
    /* 0.5 */
    auto fd = DetectorFactory::get().getDetector("person_yolov3_npu", use_npu_delegate_person_in_iou_update3);
    EXPECT_TRUE(fd.get() != nullptr);
    EXPECT_EQ(fd->getModelName(), "FitTV_Detector_Yolov3.tflite");

    std::shared_ptr<Descriptor> descriptor = std::make_shared<Yolov3Descriptor>();
    auto foundYolov3s = std::dynamic_pointer_cast<Yolov3Descriptor>(descriptor);

    for (auto i = 1; i < 10; i++) {
        std::string inputPathDir = R"(/usr/share/aif/images/pose_model_val_data/)";
        inputPathDir += std::to_string(i);
        std::vector<cv::String> files;
        cv::glob( inputPathDir, files );

        for (int j = 0; j < files.size(); j++) {
            EXPECT_TRUE(fd->detectFromImage(files[j], descriptor) == aif::kAifOk);
        }
        foundYolov3s->drawBbox(files[0]);
        std::cout << "files[j] " << files[0] << " drawBbox done!\n";
    }

    EXPECT_LT(foundYolov3s->size(), 10);
}
