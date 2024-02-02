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
#if defined(USE_O24)
    Pose3dDescriptor jpdTraj;
#endif

    std::vector<Joint3D> joints3d;
    for (int i=0; i<41; i++) {
        joints3d.push_back({i+0.0f, i+0.1f, i+0.2f});
    }
    EXPECT_EQ(joints3d.size(), 41);

    Joint3D joint3dPos = {1.0, 2.0, 3.0};

#if defined(USE_O24)
    jpd.addJoints3D(joints3d);
    jpdTraj.addTraj3D(joint3dPos);
#else
    jpd.addJointsAndTraj3D(joints3d, joint3dPos);
#endif

    auto json = jpd.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("poses3d"));
    EXPECT_TRUE(d["poses3d"].IsArray());
    EXPECT_TRUE(d["poses3d"].Size() == 1);

#if defined(USE_O24)
    auto jsonTraj = jpdTraj.toStr();
    Logd(jsonTraj);

    rj::Document dTraj;
    dTraj.Parse(jsonTraj.c_str());

    EXPECT_TRUE(dTraj.IsObject());
    EXPECT_TRUE(dTraj.HasMember("poses3d"));
    EXPECT_TRUE(dTraj["poses3d"].IsArray());
    EXPECT_TRUE(dTraj["poses3d"].Size() == 1);
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

#if defined(USE_O24)
    EXPECT_TRUE(dTraj["poses3d"][0].IsObject());
    EXPECT_TRUE(dTraj["poses3d"][0].HasMember("joint3dPos"));
    EXPECT_TRUE(dTraj["poses3d"][0]["joint3dPos"].IsArray());
    EXPECT_TRUE(dTraj["poses3d"][0]["joint3dPos"].Size() == 3);

    EXPECT_TRUE(dTraj["poses3d"][0]["joint3dPos"][0] == 1.0);
    EXPECT_TRUE(dTraj["poses3d"][0]["joint3dPos"][1] == 2.0);
    EXPECT_TRUE(dTraj["poses3d"][0]["joint3dPos"][2] == 3.0);
#else
    EXPECT_TRUE(d["poses3d"][0].HasMember("joint3dPos"));
    EXPECT_TRUE(d["poses3d"][0]["joint3dPos"].IsArray());
    EXPECT_TRUE(d["poses3d"][0]["joint3dPos"].Size() == 3);

    EXPECT_TRUE(d["poses3d"][0]["joint3dPos"][0] == 1.0);
    EXPECT_TRUE(d["poses3d"][0]["joint3dPos"][1] == 2.0);
    EXPECT_TRUE(d["poses3d"][0]["joint3dPos"][2] == 3.0);
#endif

}