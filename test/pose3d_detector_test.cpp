/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/Detector.h>
#include <aif/base/Descriptor.h>
#include <aif/bodyPoseEstimation/Pose3d/Pose3dDescriptor.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;

class Pose3dDetectorTest : public ::testing::Test
{
protected:
    Pose3dDetectorTest() = default;
    ~Pose3dDetectorTest() = default;

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
    /*std::string use_npu_delegate {
        "{"
        "  \"delegates\" : ["
        "    {"
        "      \"name\": \"npu_delegate\""
        "    }"
        "  ]"
        "}"
    };*/

    std::string use_npu_delegate {
        "{"
        "  \"delegates\" : ["
        "    {"
        "      \"name\": \"npu_delegate\","
        "      \"option\": {"
        "       }"
        "    }"
        "  ]"
        "}"};


    std::string use_npu_delegate_and_people {
        "{"
        "  \"delegates\" : ["
        "    {"
        "      \"name\": \"npu_delegate\","
        "      \"option\": {"
        "       }"
        "    }"
        "  ],"
        "  \"commonParam\" : {"
        "    \"numMaxPerson\": 5"
        "  }"
        "}"};




};

TEST_F(Pose3dDetectorTest, 01_pose3d_detect_person)
{
    auto fd = DetectorFactory::get().getDetector("pose3d_videopose3d_npu", use_npu_delegate);
    EXPECT_TRUE(fd.get() != nullptr);
    EXPECT_EQ(fd->getModelName(), "FitTV_Pose3D.tflite");
    auto modelInfo = fd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 27); // numElems
    EXPECT_EQ(modelInfo.width, 41); // mNumJointsIn
    EXPECT_EQ(modelInfo.channels, 2);

    std::shared_ptr<Descriptor> descriptor = std::make_shared<Pose3dDescriptor>();
    auto foundPose3ds = std::dynamic_pointer_cast<Pose3dDescriptor>(descriptor);

    std::vector<float> joints2d = { 290, 236,    275, 236,
                                    275, 290,    275, 350,
                                    305, 236,    305, 295,
                                    305, 350,    290, 181,
                                    290, 136,    290, 116,
                                    290, 81,     320, 146,
									330, 186,    305, 221,
                                    260, 146,    245, 186,
                                    280, 201,    290, 221,
                                    290, 186,    290, 166,
                                    290, 131,    300, 196,
                                    300, 201,    300, 201,
                                    300, 206,    295, 206,
                                    285, 231,    285, 236,
                                    285, 236,    290, 236,
					                295, 236,    270, 375,
					                265, 370,    305, 375,
				                    315, 370,    285, 101,
				                    295, 101,    275, 106,
				                    305, 106,    290, 196,
				                    295, 231
                                  };

    cv::Mat joints2d_mat(41, 2, CV_32F, joints2d.data());
    cv::Mat joints2d_matd;
    joints2d_mat.convertTo( joints2d_matd, CV_64F );

 std::cout << "Pose2D Result imageJoints[idx] " << joints2d_matd.at<double>(0,0) << std::endl;

    EXPECT_TRUE(fd->detect(joints2d_matd, descriptor) == aif::kAifOk);
    EXPECT_TRUE(fd->detect(joints2d_matd, descriptor) == aif::kAifOk);

    std::cout << foundPose3ds->toStr() << std::endl;
}

