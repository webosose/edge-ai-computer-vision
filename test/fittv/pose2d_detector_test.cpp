/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/bodyPoseEstimation/pose2d/Pose2dDescriptor.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;

class Pose2dDetectorTest : public ::testing::Test
{
protected:
    Pose2dDetectorTest() = default;
    ~Pose2dDetectorTest() = default;

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

TEST_F(Pose2dDetectorTest, 01_getModelInfo)
{
    auto fd = DetectorFactory::get().getDetector("pose2d_resnet_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    auto modelInfo = fd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 256);
    EXPECT_EQ(modelInfo.width, 192);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(Pose2dDetectorTest, 02_detect)
{
    auto pd = DetectorFactory::get().getDetector("pose2d_resnet_cpu");
    EXPECT_TRUE(pd.get() != nullptr);

    std::shared_ptr<Descriptor> descriptor = std::make_shared<Pose2dDescriptor>();
    auto pose2d = std::dynamic_pointer_cast<Pose2dDescriptor>(descriptor);
    EXPECT_TRUE(pd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(pose2d->getPoseCount(), 1);
}

TEST_F(Pose2dDetectorTest, 03_detectWithNpu)
{
    auto pd = DetectorFactory::get().getDetector("pose2d_resnet_npu");
    EXPECT_TRUE(pd.get() != nullptr);

    std::shared_ptr<Descriptor> descriptor = std::make_shared<Pose2dDescriptor>();
    auto pose2d = std::dynamic_pointer_cast<Pose2dDescriptor>(descriptor);
    EXPECT_TRUE(pd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(pose2d->getPoseCount(), 1);
    EXPECT_EQ(pose2d->getKeyPoints().size(), 41);

}
