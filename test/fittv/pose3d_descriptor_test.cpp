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

TEST_F(Pose3dDescriptorTest, 02_add_jointsandtraj)
{
    Pose3dDescriptor jpd;
#if defined(USE_FITMODEL_V2)
    Pose3dDescriptor jpdTraj;
#endif

    std::vector<Joint3D> joints3d;
    for (int i=0; i<41; i++) {
        joints3d.push_back({i+0.0f, i+0.1f, i+0.2f});
    }
    EXPECT_EQ(joints3d.size(), 41);

    Joint3D joint3dPos = {1.0f, 2.0f, 3.0f};

    jpd.addJoints3D(joints3d);
#if defined(USE_FITMODEL_V2)
    jpdTraj.addTraj3D(joint3dPos);
#else
    jpd.addTraj3D(joint3dPos);
#endif

    auto json = jpd.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("poses3d"));
    EXPECT_TRUE(d["poses3d"].IsArray());

#if defined(USE_FITMODEL_V2)
    auto jsonTraj = jpdTraj.toStr();
    Logd(jsonTraj);

    rj::Document dTraj;
    dTraj.Parse(jsonTraj.c_str());

    EXPECT_TRUE(dTraj.IsObject());
    EXPECT_TRUE(dTraj.HasMember("poses3d"));
    EXPECT_TRUE(dTraj["poses3d"].IsArray());

    EXPECT_TRUE(dTraj["poses3d"].Size() == 1);
#else
    EXPECT_TRUE(d["poses3d"].Size() == 2);
#endif

    EXPECT_TRUE(d["poses3d"][0].IsObject());
    EXPECT_TRUE(d["poses3d"][0].HasMember("joints3d"));
    EXPECT_TRUE(d["poses3d"][0]["joints3d"].IsArray());
    EXPECT_EQ(d["poses3d"][0]["joints3d"].Size(), 41);
    EXPECT_TRUE(d["poses3d"][0]["joints3d"][0].IsArray());
    EXPECT_EQ(d["poses3d"][0]["joints3d"][0].Size(), 3);

    int i;
    for ( i = 0; i < 41; i++) {
        EXPECT_TRUE(d["poses3d"][0]["joints3d"][i][0].GetFloat() - (i+0.0f) < aif::EPSILON);
        EXPECT_TRUE(d["poses3d"][0]["joints3d"][i][1].GetFloat() - (i+0.1f) < aif::EPSILON);
        EXPECT_TRUE(d["poses3d"][0]["joints3d"][i][2].GetFloat() - (i+0.2f) < aif::EPSILON);
    }

#if defined(USE_FITMODEL_V2)
    EXPECT_TRUE(dTraj["poses3d"][0].IsObject());
    EXPECT_TRUE(dTraj["poses3d"][0].HasMember("joint3dTraj"));
    EXPECT_TRUE(dTraj["poses3d"][0]["joint3dTraj"].IsArray());
    EXPECT_TRUE(dTraj["poses3d"][0]["joint3dTraj"].Size() == 3);

    EXPECT_TRUE(dTraj["poses3d"][0]["joint3dTraj"][0] == 1.0);
    EXPECT_TRUE(dTraj["poses3d"][0]["joint3dTraj"][1] == 2.0);
    EXPECT_TRUE(dTraj["poses3d"][0]["joint3dTraj"][2] == 3.0);
#else
    EXPECT_TRUE(d["poses3d"][1].HasMember("joint3dTraj"));
    EXPECT_TRUE(d["poses3d"][1]["joint3dTraj"].IsArray());
    EXPECT_TRUE(d["poses3d"][1]["joint3dTraj"].Size() == 3);

    EXPECT_TRUE(d["poses3d"][1]["joint3dTraj"][0] == 1.0);
    EXPECT_TRUE(d["poses3d"][1]["joint3dTraj"][1] == 2.0);
    EXPECT_TRUE(d["poses3d"][1]["joint3dTraj"][2] == 3.0);
#endif

}

//FIXME: @Jooyeon Lee: this doesn't exist in fittv3 branch. Need to check if it's needed
TEST_F(Pose3dDescriptorTest, 03_add_response_and_returncode)
{
    Pose3dDescriptor jpd;

    jpd.addResponseName("person_detect");
    jpd.addReturnCode(kAifOk);

    std::vector<Joint3D> joints3d;
    for (int i=0; i<41; i++) {
        joints3d.push_back({i+0.0f, i+0.1f, i+0.2f});
    }
    EXPECT_EQ(joints3d.size(), 41);

    Joint3D joint3dPos = {1.0f, 2.0f, 3.0f};

    jpd.addJoints3D(joints3d, joint3dPos);

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
