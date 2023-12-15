/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/bgSegment/BgSegmentDescriptor.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;

class BgSegmentDetectorTest : public ::testing::Test
{
protected:
    BgSegmentDetectorTest() = default;
    ~BgSegmentDetectorTest() = default;

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

TEST_F(BgSegmentDetectorTest, WithoutRoiTest)
{
    auto dt = DetectorFactory::get().getDetector("bg_segmentation_npu");
    EXPECT_TRUE(dt.get() != nullptr);

    std::shared_ptr<Descriptor> descriptor = std::make_shared<BgSegmentDescriptor>();
    EXPECT_FALSE(descriptor->hasMember("segments"));
    EXPECT_TRUE(dt->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_TRUE(descriptor->hasMember("segments"));
}

TEST_F(BgSegmentDetectorTest, WithoutRoiTest2)
{
    auto dt = DetectorFactory::get().getDetector("bg_segmentation_npu");
    EXPECT_TRUE(dt.get() != nullptr);

    std::shared_ptr<Descriptor> descriptor = std::make_shared<BgSegmentDescriptor>();
    EXPECT_FALSE(descriptor->hasMember("segments"));
    EXPECT_TRUE(dt->detectFromImage(basePath + "/images/Stretching.jpg", descriptor) == aif::kAifOk); // 1280 x 720
    EXPECT_TRUE(descriptor->hasMember("segments"));
}

TEST_F(BgSegmentDetectorTest, WithRoiTest)
{
    std::string option = R"(
        {
            "modelParam": {
                "origImgRoiX": 20,
                "origImgRoiY": 20,
                "origImgRoiWidth": 380,
                "origImgRoiHeight": 550
            }
        })";
    auto dt = DetectorFactory::get().getDetector("bg_segmentation_npu", option);
    EXPECT_TRUE(dt.get() != nullptr);

    std::shared_ptr<Descriptor> descriptor = std::make_shared<BgSegmentDescriptor>();
    EXPECT_FALSE(descriptor->hasMember("segments"));
    EXPECT_TRUE(dt->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_TRUE(descriptor->hasMember("segments"));
}

TEST_F(BgSegmentDetectorTest, WithRoiTest2)
{
    std::string option = R"(
        {
            "modelParam": {
                "origImgRoiX": 280,
                "origImgRoiY": 0,
                "origImgRoiWidth": 1000,
                "origImgRoiHeight": 720
            }
        })";
    auto dt = DetectorFactory::get().getDetector("bg_segmentation_npu", option);
    EXPECT_TRUE(dt.get() != nullptr);

    std::shared_ptr<Descriptor> descriptor = std::make_shared<BgSegmentDescriptor>();
    EXPECT_FALSE(descriptor->hasMember("segments"));
    EXPECT_TRUE(dt->detectFromImage(basePath + "/images/Stretching.jpg", descriptor) == aif::kAifOk); // 1280 x 720
    EXPECT_TRUE(descriptor->hasMember("segments"));
}
