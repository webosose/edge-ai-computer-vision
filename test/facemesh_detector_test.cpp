/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/DelegateFactory.h>
#include <aif/faceMesh/FaceMeshDescriptor.h>

#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;

class FaceMeshDetectorTest : public ::testing::Test
{
protected:
    FaceMeshDetectorTest() = default;
    ~FaceMeshDetectorTest() = default;

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

TEST_F(FaceMeshDetectorTest, 01_cpu_init)
{
    auto fmd = DetectorFactory::get().getDetector("facemesh_cpu");
    EXPECT_TRUE(fmd.get() != nullptr);
    EXPECT_EQ(fmd->getModelName(), "face_landmark.tflite");
    auto modelInfo = fmd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 192);
    EXPECT_EQ(modelInfo.width, 192);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(FaceMeshDetectorTest, 02_cpu_from_mona_face)
{
    auto fmd = DetectorFactory::get().getDetector("facemesh_cpu");
    EXPECT_TRUE(fmd.get() != nullptr);

    FaceMeshDescriptor *faceMeshDescriptor = new FaceMeshDescriptor();
    std::shared_ptr<Descriptor> descriptor(faceMeshDescriptor);
    EXPECT_TRUE(fmd->detectFromImage(basePath + "/images/mona_face.jpg", descriptor) == aif::kAifOk);
}
