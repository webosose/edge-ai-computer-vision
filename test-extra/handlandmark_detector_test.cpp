/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/handLandmark/HandLandmarkDescriptor.h>

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

class HandLandmarkDetectorTest : public ::testing::Test
{
protected:
    HandLandmarkDetectorTest() = default;
    ~HandLandmarkDetectorTest() = default;

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

TEST_F(HandLandmarkDetectorTest, 01_cpu_init)
{
    auto pd = DetectorFactory::get().getDetector("handlandmark_lite_cpu");
    ASSERT_TRUE(pd.get() != nullptr);
    ASSERT_EQ(pd->getModelName(), "hand_landmark_lite.tflite");
    auto modelInfo = pd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 224);
    EXPECT_EQ(modelInfo.width, 224);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(HandLandmarkDetectorTest, 02_cpu_from_hand)
{
    auto pd = DetectorFactory::get().getDetector("handlandmark_lite_cpu");
    ASSERT_TRUE(pd.get() != nullptr);

    HandLandmarkDescriptor* handlandmarkDescriptor = new HandLandmarkDescriptor();
    std::shared_ptr<Descriptor> descriptor(handlandmarkDescriptor);
    EXPECT_FALSE(descriptor->hasMember("hands"));
    ASSERT_TRUE(pd->detectFromImage(basePath + "/images/hand_right.jpg", descriptor) == aif::kAifOk);
    ASSERT_TRUE(descriptor->hasMember("hands"));

    auto json = descriptor->toStr();
    rj::Document d;
    d.Parse(json.c_str());
    EXPECT_TRUE(d["hands"][0]["presence"].GetFloat() > 0.5);
}

TEST_F(HandLandmarkDetectorTest, 03_cpu_from_no_hand)
{
    auto pd = DetectorFactory::get().getDetector("handlandmark_lite_cpu");
    ASSERT_TRUE(pd.get() != nullptr);

    HandLandmarkDescriptor* handlandmarkDescriptor = new HandLandmarkDescriptor();
    std::shared_ptr<Descriptor> descriptor(handlandmarkDescriptor);
    EXPECT_FALSE(descriptor->hasMember("hands"));
    ASSERT_TRUE(pd->detectFromImage(basePath + "/images/hand_none.jpg", descriptor) == aif::kAifOk);
    ASSERT_TRUE(descriptor->hasMember("hands"));

    auto json = descriptor->toStr();
    rj::Document d;
    d.Parse(json.c_str());
    EXPECT_TRUE(d["hands"][0]["presence"].GetFloat() < 0.5);
}


TEST_F(HandLandmarkDetectorTest, 04_cpu_from_multi_hand)
{
    auto pd = DetectorFactory::get().getDetector("handlandmark_lite_cpu");
    ASSERT_TRUE(pd.get() != nullptr);

    HandLandmarkDescriptor* handlandmarkDescriptor = new HandLandmarkDescriptor();
    std::shared_ptr<Descriptor> descriptor(handlandmarkDescriptor);
    EXPECT_FALSE(descriptor->hasMember("hands"));
    ASSERT_TRUE(pd->detectFromImage(basePath + "/images/hand_two.jpg", descriptor) == aif::kAifOk);
    ASSERT_TRUE(descriptor->hasMember("hands"));

    auto json = descriptor->toStr();
    rj::Document d;
    d.Parse(json.c_str());
    EXPECT_TRUE(d["hands"][0]["presence"].GetFloat() > 0.5);
}


TEST_F(HandLandmarkDetectorTest, 05_cpu_from_right_hand)
{
    auto pd = DetectorFactory::get().getDetector("handlandmark_lite_cpu");
    ASSERT_TRUE(pd.get() != nullptr);

    HandLandmarkDescriptor* handlandmarkDescriptor = new HandLandmarkDescriptor();
    std::shared_ptr<Descriptor> descriptor(handlandmarkDescriptor);
    EXPECT_FALSE(descriptor->hasMember("hands"));
    ASSERT_TRUE(pd->detectFromImage(basePath + "/images/hand_right.jpg", descriptor) == aif::kAifOk);
    ASSERT_TRUE(descriptor->hasMember("hands"));

    auto json = descriptor->toStr();
    rj::Document d;
    d.Parse(json.c_str());
    EXPECT_TRUE(d["hands"][0]["presence"].GetFloat() > 0.5);
    EXPECT_TRUE(d["hands"][0]["handedness"].GetFloat() > 0.5);
}

TEST_F(HandLandmarkDetectorTest, 06_cpu_from_left_hand)
{
    auto pd = DetectorFactory::get().getDetector("handlandmark_lite_cpu");
    ASSERT_TRUE(pd.get() != nullptr);

    HandLandmarkDescriptor* handlandmarkDescriptor = new HandLandmarkDescriptor();
    std::shared_ptr<Descriptor> descriptor(handlandmarkDescriptor);
    EXPECT_FALSE(descriptor->hasMember("hands"));
    ASSERT_TRUE(pd->detectFromImage(basePath + "/images/hand_left.jpg", descriptor) == aif::kAifOk);
    ASSERT_TRUE(descriptor->hasMember("hands"));

    auto json = descriptor->toStr();
    rj::Document d;
    d.Parse(json.c_str());
    EXPECT_TRUE(d["hands"][0]["presence"].GetFloat() > 0.5);
    EXPECT_TRUE(d["hands"][0]["handedness"].GetFloat() < 0.5);
}

TEST_F(HandLandmarkDetectorTest, 07_cpu_full_from_hand)
{
    auto pd = DetectorFactory::get().getDetector("handlandmark_full_cpu");
    ASSERT_TRUE(pd.get() != nullptr);

    HandLandmarkDescriptor* handlandmarkDescriptor = new HandLandmarkDescriptor();
    std::shared_ptr<Descriptor> descriptor(handlandmarkDescriptor);
    EXPECT_FALSE(descriptor->hasMember("hands"));
    ASSERT_TRUE(pd->detectFromImage(basePath + "/images/hand_right.jpg", descriptor) == aif::kAifOk);
    ASSERT_TRUE(descriptor->hasMember("hands"));

    auto json = descriptor->toStr();
    rj::Document d;
    d.Parse(json.c_str());
    EXPECT_TRUE(d["hands"][0]["presence"].GetFloat() > 0.5);
}

TEST_F(HandLandmarkDetectorTest, 08_cpu_full_from_no_hand)
{
    auto pd = DetectorFactory::get().getDetector("handlandmark_full_cpu");
    ASSERT_TRUE(pd.get() != nullptr);

    HandLandmarkDescriptor* handlandmarkDescriptor = new HandLandmarkDescriptor();
    std::shared_ptr<Descriptor> descriptor(handlandmarkDescriptor);
    EXPECT_FALSE(descriptor->hasMember("hands"));
    ASSERT_TRUE(pd->detectFromImage(basePath + "/images/hand_none.jpg", descriptor) == aif::kAifOk);
    ASSERT_TRUE(descriptor->hasMember("hands"));

    auto json = descriptor->toStr();
    rj::Document d;
    d.Parse(json.c_str());
    EXPECT_TRUE(d["hands"][0]["presence"].GetFloat() < 0.5);
}


TEST_F(HandLandmarkDetectorTest, 09_cpu_full_from_multi_hand)
{
    auto pd = DetectorFactory::get().getDetector("handlandmark_full_cpu");
    ASSERT_TRUE(pd.get() != nullptr);

    HandLandmarkDescriptor* handlandmarkDescriptor = new HandLandmarkDescriptor();
    std::shared_ptr<Descriptor> descriptor(handlandmarkDescriptor);
    EXPECT_FALSE(descriptor->hasMember("hands"));
    ASSERT_TRUE(pd->detectFromImage(basePath + "/images/hand_two.jpg", descriptor) == aif::kAifOk);
    ASSERT_TRUE(descriptor->hasMember("hands"));

    auto json = descriptor->toStr();
    rj::Document d;
    d.Parse(json.c_str());
    EXPECT_TRUE(d["hands"][0]["presence"].GetFloat() > 0.5);
}


TEST_F(HandLandmarkDetectorTest, 10_cpu_full_from_right_hand)
{
    auto pd = DetectorFactory::get().getDetector("handlandmark_full_cpu");
    ASSERT_TRUE(pd.get() != nullptr);

    HandLandmarkDescriptor* handlandmarkDescriptor = new HandLandmarkDescriptor();
    std::shared_ptr<Descriptor> descriptor(handlandmarkDescriptor);
    EXPECT_FALSE(descriptor->hasMember("hands"));
    ASSERT_TRUE(pd->detectFromImage(basePath + "/images/hand_right.jpg", descriptor) == aif::kAifOk);
    ASSERT_TRUE(descriptor->hasMember("hands"));

    auto json = descriptor->toStr();
    rj::Document d;
    d.Parse(json.c_str());
    EXPECT_TRUE(d["hands"][0]["presence"].GetFloat() > 0.5);
    EXPECT_TRUE(d["hands"][0]["handedness"].GetFloat() > 0.5);
}

TEST_F(HandLandmarkDetectorTest, 11_cpu_full_from_left_hand)
{
    auto pd = DetectorFactory::get().getDetector("handlandmark_full_cpu");
    ASSERT_TRUE(pd.get() != nullptr);

    HandLandmarkDescriptor* handlandmarkDescriptor = new HandLandmarkDescriptor();
    std::shared_ptr<Descriptor> descriptor(handlandmarkDescriptor);
    EXPECT_FALSE(descriptor->hasMember("hands"));
    ASSERT_TRUE(pd->detectFromImage(basePath + "/images/hand_left.jpg", descriptor) == aif::kAifOk);
    ASSERT_TRUE(descriptor->hasMember("hands"));

    auto json = descriptor->toStr();
    rj::Document d;
    d.Parse(json.c_str());
    EXPECT_TRUE(d["hands"][0]["presence"].GetFloat() > 0.5);
    EXPECT_TRUE(d["hands"][0]["handedness"].GetFloat() < 0.5);
}
