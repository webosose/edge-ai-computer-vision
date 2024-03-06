/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */
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

class YuNetFaceDetectorTest : public ::testing::Test
{
protected:
    YuNetFaceDetectorTest() = default;
    ~YuNetFaceDetectorTest() = default;

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

TEST_F(YuNetFaceDetectorTest, 01_yunet_init)
{
    auto fd = DetectorFactory::get().getDetector("face_yunet_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    EXPECT_EQ(fd->getModelName(), "face_yunet.tflite");
    auto modelInfo = fd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 120);
    EXPECT_EQ(modelInfo.width, 160);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(YuNetFaceDetectorTest, 02_faces_from_5faces)
{
    auto fd = DetectorFactory::get().getDetector("face_yunet_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/5faces.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 5);
}

TEST_F(YuNetFaceDetectorTest, 03_faces_from_2faces)
{
    auto fd = DetectorFactory::get().getDetector("face_yunet_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/2faces.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 2);
}
