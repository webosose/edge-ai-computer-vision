#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/Detector.h>
#include <aif/base/Descriptor.h>
#include <aif/palm/PalmDescriptor.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;

class PalmDetectorTest : public ::testing::Test
{
protected:
    PalmDetectorTest() = default;
    ~PalmDetectorTest() = default;

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

TEST_F(PalmDetectorTest, 01_palm_lite_init)
{
    auto fd = DetectorFactory::get().getDetector("palm_lite_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    EXPECT_EQ(fd->getModelName(), "palm_detection_lite.tflite");
    auto modelInfo = fd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 192);
    EXPECT_EQ(modelInfo.width, 192);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(PalmDetectorTest, 02_palm_lite_from_one_hand)
{
    auto fd = DetectorFactory::get().getDetector("palm_lite_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<PalmDescriptor>();
    auto foundPalms = std::dynamic_pointer_cast<PalmDescriptor>(descriptor);
    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/hand_right.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundPalms->size(), 1);
}

TEST_F(PalmDetectorTest, 03_palm_lite_from_two_hand)
{
    auto fd = DetectorFactory::get().getDetector("palm_lite_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<PalmDescriptor>();
    auto foundPalms = std::dynamic_pointer_cast<PalmDescriptor>(descriptor);
    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/hand_two.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundPalms->size(), 2);
}

TEST_F(PalmDetectorTest, 04_palm_lite_from_no_hand)
{
    auto fd = DetectorFactory::get().getDetector("palm_lite_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<PalmDescriptor>();
    auto foundPalms = std::dynamic_pointer_cast<PalmDescriptor>(descriptor);
    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/hand_none.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundPalms->size(), 0);
}

TEST_F(PalmDetectorTest, 05_palm_full_init)
{
    auto fd = DetectorFactory::get().getDetector("palm_full_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    EXPECT_EQ(fd->getModelName(), "palm_detection_full.tflite");
    auto modelInfo = fd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 192);
    EXPECT_EQ(modelInfo.width, 192);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(PalmDetectorTest, 06_palm_full_from_one_hand)
{
    auto fd = DetectorFactory::get().getDetector("palm_full_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<PalmDescriptor>();
    auto foundPalms = std::dynamic_pointer_cast<PalmDescriptor>(descriptor);
    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/hand_right.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundPalms->size(), 1);
}

TEST_F(PalmDetectorTest, 07_palm_full_from_two_hand)
{
    auto fd = DetectorFactory::get().getDetector("palm_full_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<PalmDescriptor>();
    auto foundPalms = std::dynamic_pointer_cast<PalmDescriptor>(descriptor);
    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/hand_two.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundPalms->size(), 2);
}

TEST_F(PalmDetectorTest, 08_palm_full_from_no_hand)
{
    auto fd = DetectorFactory::get().getDetector("palm_full_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<PalmDescriptor>();
    auto foundPalms = std::dynamic_pointer_cast<PalmDescriptor>(descriptor);
    EXPECT_TRUE(fd->detectFromImage(basePath + "/images/hand_none.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(foundPalms->size(), 0);
}

