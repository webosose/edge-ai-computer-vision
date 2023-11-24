/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/pose3d/Pose3dDescriptor.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;
namespace rj = rapidjson;

class Pose3dDescriptorTest : public ::testing::Test
{
protected:
    Pose3dDescriptorTest() = default;
    ~Pose3dDescriptorTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(Pose3dDescriptorTest, 01_constructor)
{
    Pose3dDescriptor jpd;
    auto json = jpd.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
}

TEST_F(Pose3dDescriptorTest, 02_addjoints3d_one)
{
    Pose3dDescriptor jpd;

    std::vector<Joint3D> joints3d;
    for (int i=0; i<41; i++) {
        joints3d.push_back({i+0.0, i+0.1, i+0.2});
    }
    EXPECT_EQ(joints3d.size(), 41);

    Joint3D joint3dPos = {1.0, 2.0, 3.0};

    jpd.addJointsAndTraj3D(joints3d, joint3dPos);

    auto json = jpd.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("poses3d"));
    EXPECT_TRUE(d["poses3d"].IsArray());
    EXPECT_TRUE(d["poses3d"].Size() == 1);
    EXPECT_TRUE(d["poses3d"][0].IsObject());
    EXPECT_TRUE(d["poses3d"][0].HasMember("joints3d"));
    EXPECT_TRUE(d["poses3d"][0]["joints3d"].IsArray());
    EXPECT_EQ(d["poses3d"][0]["joints3d"].Size(), 41);
    EXPECT_TRUE(d["poses3d"][0]["joints3d"][0].IsArray());
    EXPECT_EQ(d["poses3d"][0]["joints3d"][0].Size(), 3);

    int i;
    for ( i = 0; i < 41; i++) {
        EXPECT_TRUE(d["poses3d"][0]["joints3d"][i][0].GetDouble() - (i+0.0) < aif::EPSILON);
        EXPECT_TRUE(d["poses3d"][0]["joints3d"][i][1].GetDouble() - (i+0.1) < aif::EPSILON);
        EXPECT_TRUE(d["poses3d"][0]["joints3d"][i][2].GetDouble() - (i+0.2) < aif::EPSILON);
    }
    EXPECT_TRUE(d["poses3d"][0].HasMember("joint3dPos"));
    EXPECT_TRUE(d["poses3d"][0]["joint3dPos"].IsArray());
    EXPECT_TRUE(d["poses3d"][0]["joint3dPos"].Size() == 3);

    EXPECT_TRUE(d["poses3d"][0]["joint3dPos"][0] == 1.0);
    EXPECT_TRUE(d["poses3d"][0]["joint3dPos"][1] == 2.0);
    EXPECT_TRUE(d["poses3d"][0]["joint3dPos"][2] == 3.0);
}


TEST_F(Pose3dDescriptorTest, 03_add_response_and_returncode)
{
    Pose3dDescriptor jpd;

    jpd.addResponseName("person_detect");
    jpd.addReturnCode(kAifOk);

    std::vector<Joint3D> joints3d;
    for (int i=0; i<41; i++) {
        joints3d.push_back({i+0.0, i+0.1, i+0.2});
    }
    EXPECT_EQ(joints3d.size(), 41);

    Joint3D joint3dPos = {1.0, 2.0, 3.0};

    jpd.addJointsAndTraj3D(joints3d, joint3dPos);

    auto json = jpd.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("response"));
    EXPECT_TRUE(d.HasMember("returnCode"));
    EXPECT_TRUE(d.HasMember("poses3d"));
    EXPECT_TRUE(d["poses3d"].IsArray());
    EXPECT_TRUE(d["poses3d"].Size() == 1);
}

