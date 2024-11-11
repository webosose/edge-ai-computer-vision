/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DelegateFactory.h>

#include <aifex/face/FaceDescriptor.h>
#ifndef USE_UPDATABLE_MODELS
#include <aifex/pose/PosenetDescriptor.h>
#endif

#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

#include <gtest/gtest.h>
#include <aif/auto_delegation/AutoDelegateSelector.h>


typedef aif::AutoDelegateSelector ADS;
typedef aif::AccelerationPolicyManager APM;

using namespace aif;

class AutoDelegateTest : public ::testing::Test
{
protected:
    AutoDelegateTest() = default;
    ~AutoDelegateTest() = default;

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
        basePath = AIVision::getBasePath();
    }

    void TearDown() override
    {
        DetectorFactory::get().clear();
    }

    std::string basePath;
    std::string use_auto_delegate {
        "{"
        "  \"autoDelegate\" : {"
        "      \"policy\": \"PYTORCH_MODEL_GPU\","
        "      \"cpu_fallback_percentage\": 15 "
        "    }"
        "}"};

    std::string not_use_auto_delegate {
        "{"
        "  \"delegates\" : ["
        "    {"
        "      \"name\": \"gpu_delegate\","
        "      \"option\": {"
        "           \"inference_preference\": \"SUSTAINED_SPEED\" ,"
        "           \"inference_priorities\": [\"MIN_LATENCY\", "
        "           \"MIN_MEMORY_USAGE\", \"MAX_PRECISION\"], "
        "           \"enable_quantized_inference\" : true"
        "       }"
        "     }"
        "  ]"
        "}"};

};


TEST_F(AutoDelegateTest, 01_use_default)
{
    auto fd = DetectorFactory::get().getDetector("face_yunet_360_640");
    ASSERT_TRUE(fd.get() != nullptr);

    FaceDescriptor* faceDescriptor = new FaceDescriptor();
    std::shared_ptr<Descriptor> descriptor(faceDescriptor);
    EXPECT_EQ(faceDescriptor->size(), 0);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(faceDescriptor->size(), 1);
    EXPECT_TRUE(fd->getAutoDelegateMode());
}

#ifndef USE_HOST_TEST
#ifndef USE_UPDATABLE_MODELS
TEST_F(AutoDelegateTest, 02_not_use_auto_delegate)
{
    auto pd = DetectorFactory::get().getDetector("posenet_mobilenet_cpu", not_use_auto_delegate);
    EXPECT_TRUE(pd.get() != nullptr);

    PosenetDescriptor* poseDescriptor = new PosenetDescriptor();
    std::shared_ptr<Descriptor> descriptor(poseDescriptor);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 0);
    EXPECT_TRUE(pd->detectFromImage(basePath + "/images/people.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 5);
    EXPECT_FALSE(pd->getAutoDelegateMode());
}
#endif

TEST_F(AutoDelegateTest, 03_use_auto_delegate)
{
    std::shared_ptr<Detector> fd = DetectorFactory::get().getDetector("face_yunet_360_640", use_auto_delegate);
    ASSERT_TRUE(fd.get() != nullptr);

    FaceDescriptor* faceDescriptor = new FaceDescriptor();
    std::shared_ptr<Descriptor> descriptor(faceDescriptor);
    EXPECT_EQ(faceDescriptor->size(), 0);
    ASSERT_TRUE(fd->detectFromImage(basePath + "/images/people.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(faceDescriptor->size(), 5);
    EXPECT_TRUE(fd->getAutoDelegateMode());
}
#endif
