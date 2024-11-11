/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/pose/PosenetDescriptor.h>

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

#ifdef USE_EDGETPU
#include <edgetpu.h>
#endif

using namespace aif;

class PosenetDetectorTest : public ::testing::Test
{
protected:
    PosenetDetectorTest() = default;
    ~PosenetDetectorTest() = default;

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
    std::string armnn_delegate_cpuref_param {
        "{"
        "  \"common\" : {"
        "      \"useXnnpack\": true,"
        "      \"numThreads\": 1"
        "    },"
        "  \"delegates\" : ["
        "    {"
        "      \"name\": \"armnn_delegate\","
        "      \"option\": {"
        "        \"backends\": [\"CpuRef\"],"
        "        \"logging-severity\": \"info\""
        "      }"
        "    }"
        "  ]"
        "}"
    };
    std::string armnn_delegate_cpuacc_param {
        "{"
        "  \"common\" : {"
        "      \"useXnnpack\": true,"
        "      \"numThreads\": 1"
        "    },"
        "  \"delegates\" : ["
        "    {"
        "      \"name\": \"armnn_delegate\","
        "      \"option\": {"
        "        \"backends\": [\"CpuAcc\"],"
        "        \"logging-severity\": \"info\""
        "      }"
        "    }"
        "  ]"
        "}"
    };


};


TEST_F(PosenetDetectorTest, 01_cpu_init)
{
    auto pd = DetectorFactory::get().getDetector("posenet_mobilenet_cpu");
    ASSERT_TRUE(pd.get() != nullptr);
    ASSERT_EQ(pd->getModelName(), "posenet_mobilenet_v1_075_353_481_quant_decoder.tflite");
    auto modelInfo = pd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 353);
    EXPECT_EQ(modelInfo.width, 481);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(PosenetDetectorTest, 02_cpu_from_person)
{
    auto pd = DetectorFactory::get().getDetector("posenet_mobilenet_cpu");
    ASSERT_TRUE(pd.get() != nullptr);

    PosenetDescriptor* poseDescriptor = new PosenetDescriptor();
    std::shared_ptr<Descriptor> descriptor(poseDescriptor);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 0);
    ASSERT_TRUE(pd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 1);
}


TEST_F(PosenetDetectorTest, 03_cpu_from_people)
{
    auto pd = DetectorFactory::get().getDetector("posenet_mobilenet_cpu");
    ASSERT_TRUE(pd.get() != nullptr);

    PosenetDescriptor* poseDescriptor = new PosenetDescriptor();
    std::shared_ptr<Descriptor> descriptor(poseDescriptor);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 0);
    ASSERT_TRUE(pd->detectFromImage(basePath + "/images/people.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 5);
}

TEST_F(PosenetDetectorTest, 04_cpu_posenet_from_base64_person)
{
    auto pd = DetectorFactory::get().getDetector("posenet_mobilenet_cpu");
    ASSERT_TRUE(pd.get() != nullptr);

    auto base64image = aif::fileToStr(basePath + "/images/mona_base64.jpg"); // 128 x 128
    PosenetDescriptor* poseDescriptor = new PosenetDescriptor();
    std::shared_ptr<Descriptor> descriptor(poseDescriptor);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 0);
    ASSERT_TRUE(pd->detectFromBase64(base64image, descriptor) == aif::kAifOk);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 1);
}

#ifdef USE_EDGETPU
TEST_F(PosenetDetectorTest, 05_edgetpu_from_person)
{
    // Sets up the edgetpu_context. available for any 1 TPU device.
    auto edgetpu = edgetpu::EdgeTpuManager::GetSingleton()->OpenDevice();
    if (edgetpu == nullptr) {
        Loge("can't get edgetpu context!! Skip this test");
        SUCCEED();
        return;
    }
    edgetpu.reset();

    auto pd = DetectorFactory::get().getDetector("posenet_mobilenet_edgetpu");
    ASSERT_TRUE(pd.get() != nullptr);

    PosenetDescriptor* poseDescriptor = new PosenetDescriptor();
    std::shared_ptr<Descriptor> descriptor(poseDescriptor);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 0);
    ASSERT_TRUE(pd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 1);
}
#endif

#ifdef USE_ARMNN

TEST_F(PosenetDetectorTest, 06_armnn_posenet_from_person_default)
{
    auto pd = DetectorFactory::get().getDetector("posenet_mobilenet_cpu", armnn_delegate_cpuref_param);
    ASSERT_TRUE(pd.get() != nullptr);
    EXPECT_TRUE(pd->getNumDelegates() == 1);

    PosenetDescriptor* poseDescriptor = new PosenetDescriptor();
    std::shared_ptr<Descriptor> descriptor(poseDescriptor);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 0);
    ASSERT_TRUE(pd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 1);
}

TEST_F(PosenetDetectorTest, 07_armnn_posenet_from_person_cpuAcc)
{
    auto pd = DetectorFactory::get().getDetector("posenet_mobilenet_cpu", armnn_delegate_cpuacc_param);
    ASSERT_TRUE(pd.get() != nullptr);
    EXPECT_TRUE(pd->getNumDelegates() == 1);

    PosenetDescriptor* poseDescriptor = new PosenetDescriptor();
    std::shared_ptr<Descriptor> descriptor(poseDescriptor);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 0);
    ASSERT_TRUE(pd->detectFromImage(basePath + "/images/person.jpg", descriptor) == aif::kAifOk);
    EXPECT_EQ(poseDescriptor->getPoseCount(), 1);
}
#endif
