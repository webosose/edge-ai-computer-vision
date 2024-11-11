/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */
#include <aifex/face/FaceDetector.h>
#include <aifex/face/FaceDescriptor.h>

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

class FaceDetectorTest : public ::testing::Test
{
protected:
    FaceDetectorTest() = default;
    ~FaceDetectorTest() = default;

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

TEST_F(FaceDetectorTest, 01_short_range_init)
{
    auto fd = DetectorFactory::get().getDetector("face_short_range_cpu");
    ASSERT_TRUE(fd.get() != nullptr);
    ASSERT_EQ(fd->getModelName(), "face_detection_short_range.tflite");
    auto modelInfo = fd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 128);
    EXPECT_EQ(modelInfo.width, 128);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(FaceDetectorTest, 02_short_range_detect_faces_from_mona)
{
    auto fd = DetectorFactory::get().getDetector("face_short_range_cpu");
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/mona.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

TEST_F(FaceDetectorTest, 03_short_range_detect_faces_from_5faces)
{
    auto fd = DetectorFactory::get().getDetector("face_short_range_cpu");
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/5faces.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 5);
}

TEST_F(FaceDetectorTest, 04_short_range_detect_faces_from_2faces)
{
    auto fd = DetectorFactory::get().getDetector("face_short_range_cpu");
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/2faces.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 2);
}

TEST_F(FaceDetectorTest, 05_short_range_detect_faces_from_base64_mona)
{
    auto fd = DetectorFactory::get().getDetector("face_short_range_cpu");
    ASSERT_TRUE(fd.get() != nullptr);

    auto base64image = aif::fileToStr(basePath + "/images/mona_base64.jpg"); // 128 x 128
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromBase64(base64image, descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

#ifdef USE_EDGETPU
TEST_F(FaceDetectorTest, 06_edgetpu_short_range_detect_faces_from_base64mona)
{
    auto fd = DetectorFactory::get().getDetector("face_short_range_edgetpu");
    ASSERT_TRUE(fd.get() != nullptr);

    auto base64image = aif::fileToStr(basePath + "/images/mona_base64.jpg"); // 128 x 128
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromBase64(base64image, descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}
#endif
