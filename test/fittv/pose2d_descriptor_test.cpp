/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/pose2d/Pose2dDescriptor.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;
namespace rj = rapidjson;

class Pose2dDescriptorTest : public ::testing::Test
{
protected:
    Pose2dDescriptorTest() = default;
    ~Pose2dDescriptorTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(Pose2dDescriptorTest, 01_constructor)
{
    Pose2dDescriptor pd;
    auto json = pd.toStr();

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
}

TEST_F(Pose2dDescriptorTest, 02_addKeyPoints)
{
    Pose2dDescriptor pd;

    std::vector<std::vector<float>> keyPoints;
    for (int i = 0; i < Pose2dDescriptor::KEY_POINT_TYPES; i++) {
        keyPoints.push_back({ i + 0.1f, i + 0.2f, i + 0.3f });
    }
    pd.addKeyPoints(keyPoints);

    EXPECT_EQ(pd.getPoseCount(), 1);

    rj::Document d;
    d.Parse(pd.toStr().c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("poses"));
    EXPECT_TRUE(d["poses"].IsArray());
    EXPECT_TRUE(d["poses"].Size() == 1);
    EXPECT_TRUE(d["poses"][0].IsObject());
    EXPECT_TRUE(d["poses"][0].HasMember("keypoints"));
    EXPECT_TRUE(d["poses"][0]["keypoints"].IsArray());
    EXPECT_EQ(d["poses"][0]["keypoints"].Size(), Pose2dDescriptor::KEY_POINT_TYPES);
    EXPECT_TRUE(d["poses"][0]["keypoints"][0].IsArray());
    EXPECT_EQ(d["poses"][0]["keypoints"][0].Size(), 3);

    for (int i = 0; i < Pose2dDescriptor::KEY_POINT_TYPES; i++) {
        EXPECT_TRUE(d["poses"][0]["keypoints"][i][0].GetDouble() - (i + 0.1f) < aif::EPSILON);
        EXPECT_TRUE(d["poses"][0]["keypoints"][i][1].GetDouble() - (i + 0.2f) < aif::EPSILON);
        EXPECT_TRUE(d["poses"][0]["keypoints"][i][2].GetDouble() - (i + 0.3f) < aif::EPSILON);
    }
}

TEST_F(Pose2dDescriptorTest, 03_add_response_and_returncode)
{
    Pose2dDescriptor pd;

    pd.addResponseName("pose2d_dectect");
    pd.addReturnCode(kAifOk);

    std::vector<std::vector<float>> keyPoints;
    for (int i = 0; i < Pose2dDescriptor::KEY_POINT_TYPES; i++) {
        keyPoints.push_back({ i + 0.1f, i + 0.2f, i + 0.3f });
    }
    pd.addKeyPoints(keyPoints);

    rj::Document d;
    d.Parse(pd.toStr().c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("response"));
    EXPECT_TRUE(d.HasMember("returnCode"));
    EXPECT_TRUE(d.HasMember("poses"));
    EXPECT_TRUE(d["poses"].IsArray());
    EXPECT_EQ(d["poses"].Size(), 1);
}
