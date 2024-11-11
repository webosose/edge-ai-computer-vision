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


TEST_F(YuNetFaceDetectorTest, yunet_180_320)
{
    auto fd = DetectorFactory::get().getDetector("face_yunet_180_320");
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

TEST_F(YuNetFaceDetectorTest, yunet_180_320_quant)
{
    auto fd = DetectorFactory::get().getDetector("face_yunet_180_320_quant");
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

TEST_F(YuNetFaceDetectorTest, yunet_240_320)
{
    auto fd = DetectorFactory::get().getDetector("face_yunet_240_320");
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

TEST_F(YuNetFaceDetectorTest, yunet_240_320_quant)
{
    auto fd = DetectorFactory::get().getDetector("face_yunet_240_320_quant");
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

TEST_F(YuNetFaceDetectorTest, yunet_320_320)
{
    auto fd = DetectorFactory::get().getDetector("face_yunet_320_320");
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

TEST_F(YuNetFaceDetectorTest, yunet_320_320_quant)
{
    auto fd = DetectorFactory::get().getDetector("face_yunet_320_320_quant");
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

TEST_F(YuNetFaceDetectorTest, yunet_360_640)
{
    auto fd = DetectorFactory::get().getDetector("face_yunet_360_640");
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

TEST_F(YuNetFaceDetectorTest, yunet_360_640_quant)
{
    auto fd = DetectorFactory::get().getDetector("face_yunet_360_640_quant");
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

TEST_F(YuNetFaceDetectorTest, yunet_480_640)
{
    auto fd = DetectorFactory::get().getDetector("face_yunet_480_640");
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

TEST_F(YuNetFaceDetectorTest, yunet_480_640_quant)
{
    auto fd = DetectorFactory::get().getDetector("face_yunet_480_640_quant");
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

TEST_F(YuNetFaceDetectorTest, yunet_640_640)
{
    auto fd = DetectorFactory::get().getDetector("face_yunet_640_640");
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

TEST_F(YuNetFaceDetectorTest, yunet_640_640_quant)
{
    auto fd = DetectorFactory::get().getDetector("face_yunet_640_640_quant");
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

TEST_F(YuNetFaceDetectorTest, yunet_270_480)
{
    auto fd = DetectorFactory::get().getDetector("face_yunet_270_480");
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

TEST_F(YuNetFaceDetectorTest, yunet_270_480_quant)
{
    auto fd = DetectorFactory::get().getDetector("face_yunet_270_480_quant");
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

TEST_F(YuNetFaceDetectorTest, yunet_180_320_threshold_9)
{
    std::string param =
    "{ \
        \"modelParam\": { \
            \"scoreThreshold\": 0.9, \
            \"nmsThreshold\": 0.3, \
            \"topK\": 5000 \
        } \
    }";

    auto fd = DetectorFactory::get().getDetector("face_yunet_180_320", param);
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

TEST_F(YuNetFaceDetectorTest, yunet_180_320_threshold_5)
{
    std::string param =
    "{ \
        \"modelParam\": { \
            \"scoreThreshold\": 0.5, \
            \"nmsThreshold\": 0.3, \
            \"topK\": 5000 \
        } \
    }";

    auto fd = DetectorFactory::get().getDetector("face_yunet_180_320", param);
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}

TEST_F(YuNetFaceDetectorTest, yunet_180_320_threshold_3)
{
    std::string param =
    "{ \
        \"modelParam\": { \
            \"scoreThreshold\": 0.3, \
            \"nmsThreshold\": 0.3, \
            \"topK\": 5000 \
        } \
    }";

    auto fd = DetectorFactory::get().getDetector("face_yunet_180_320", param);
    ASSERT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<FaceDescriptor>();
    auto foundFaces = std::dynamic_pointer_cast<FaceDescriptor>(descriptor);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundFaces->size(), 1);
}
