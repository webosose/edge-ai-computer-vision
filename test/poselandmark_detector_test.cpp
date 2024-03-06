/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/poseLandmark/PoseLandmarkDescriptor.h>

#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DelegateFactory.h>

#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;

class PoseLandmarkDetectorTest : public ::testing::Test
{
protected:
    PoseLandmarkDetectorTest() = default;
    ~PoseLandmarkDetectorTest() = default;

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

TEST_F(PoseLandmarkDetectorTest, 01_cpu_init)
{
    auto pd = DetectorFactory::get().getDetector("poselandmark_lite_cpu");
    EXPECT_TRUE(pd.get() != nullptr);
    EXPECT_EQ(pd->getModelName(), "pose_landmark_lite.tflite");
    auto modelInfo = pd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 256);
    EXPECT_EQ(modelInfo.width, 256);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(PoseLandmarkDetectorTest, 02_cpu_from_person)
{
    auto pd = DetectorFactory::get().getDetector("poselandmark_lite_cpu");
    EXPECT_TRUE(pd.get() != nullptr);

    PoseLandmarkDescriptor* poselandmarkDescriptor = new PoseLandmarkDescriptor();
    std::shared_ptr<Descriptor> descriptor(poselandmarkDescriptor);
    EXPECT_TRUE(descriptor->hasMember("poses"));
    EXPECT_EQ(poselandmarkDescriptor->getPoseCount(), 0);
    EXPECT_TRUE(pd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(poselandmarkDescriptor->getPoseCount(), 1);
}


TEST_F(PoseLandmarkDetectorTest, 03_cpu_from_people)
{
    auto pd = DetectorFactory::get().getDetector("poselandmark_lite_cpu");
    EXPECT_TRUE(pd.get() != nullptr);

    PoseLandmarkDescriptor* poselandmarkDescriptor = new PoseLandmarkDescriptor();
    std::shared_ptr<Descriptor> descriptor(poselandmarkDescriptor);
    EXPECT_TRUE(descriptor->hasMember("poses"));
    EXPECT_EQ(poselandmarkDescriptor->getPoseCount(), 0);
    EXPECT_TRUE(pd->detectFromImage(basePath + "/images/people.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(poselandmarkDescriptor->getPoseCount(), 1);
}

TEST_F(PoseLandmarkDetectorTest, 04_cpu_poselandmark_from_base64_mona)
{
    auto pd = DetectorFactory::get().getDetector("poselandmark_lite_cpu");
    EXPECT_TRUE(pd.get() != nullptr);

    auto base64image = aif::fileToStr(basePath + "/images/mona_base64.jpg"); // 128 x 128
    PoseLandmarkDescriptor* poselandmarkDescriptor = new PoseLandmarkDescriptor();
    std::shared_ptr<Descriptor> descriptor(poselandmarkDescriptor);
    EXPECT_TRUE(descriptor->hasMember("poses"));
    EXPECT_EQ(poselandmarkDescriptor->getPoseCount(), 0);
    EXPECT_TRUE(pd->detectFromBase64(base64image, descriptor) == aif::kAifOk);
    EXPECT_EQ(poselandmarkDescriptor->getPoseCount(), 1);
}
