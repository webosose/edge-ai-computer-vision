/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/Pose3d/Pose3dParam.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace aif;

class Pose3dParamTest : public ::testing::Test
{
protected:
    Pose3dParamTest() = default;
    ~Pose3dParamTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(Pose3dParamTest, 01_constructor)
{
    // default constructor
    Pose3dParam pp;
    pp.trace();

    Pose3dParam pp2;
    pp2.fromJson("{ \
        \"modelParam\": { \
            \"maxBatchSize\": 4, \
            \"flipPoses\": true \
        } \
    }");
    pp2.trace();

    EXPECT_EQ(pp, pp2);
}

TEST_F(Pose3dParamTest, 02_copy_constructors)
{
    // constructor 1
    Pose3dParam pp1;
    pp1.trace();

    // copy constructor
    Pose3dParam pp2(pp1);
    EXPECT_EQ(pp1, pp2);

    // copy assignment operation
    Pose3dParam pp3;
    pp3 = pp1;
    ASSERT_EQ(pp3, pp1);
}

TEST_F(Pose3dParamTest, 03_move_constructors)
{
    // constructor 1
    Pose3dParam pp1;
    pp1.trace();

    // copy constructor
    Pose3dParam pp2(pp1);
    Pose3dParam pp3(pp1);

    // move constructor
    Pose3dParam pp4(std::move(pp2));

    EXPECT_EQ(pp4, pp1);

    // move assignment operation
    Pose3dParam pp5;
    pp5 = std::move(pp3);

    EXPECT_EQ(pp5, pp1);
}

TEST_F(Pose3dParamTest, 04_test_fromJson)
{
    Pose3dParam pp;
    pp.fromJson("{ \
        \"modelParam\": { \
            \"maxBatchSize\": 4, \
            \"flipPoses\": true, \
            \"arch\": [3, 3, 3],\
            \"flipPoseMap\": [ 0, 4, 5 ],\
            \"preprocessingType\": 1, \
            \"focalLength\": [ 1469.2684222875848, 1469.8823503910792 ],\
            \"center\": [ 650.1274669098675, 505.44118528424053 ],\
            \"radialdistortion\": [ -0.12686622768237987, -0.33232941922945763, 0.3877008223664106 ],\
            \"tandistortion\": [ 0.0015259532472412637, 0.0029597555802232116 ],\
            \"pose2DJoints\":[ 290, 236,\
                               275, 236, \
                               275, 290 \
                             ]\
        } \
    }");

    EXPECT_EQ(pp.maxBatchSize, 4);
    EXPECT_EQ(pp.flipPoses, true);
    EXPECT_EQ(pp.arch[0], 3);
    EXPECT_EQ(pp.arch[1], 3);
    EXPECT_EQ(pp.arch[2], 3);
    EXPECT_EQ(pp.flipPoseMap[0], 0);
    EXPECT_EQ(pp.flipPoseMap[1], 4);
    EXPECT_EQ(pp.flipPoseMap[2], 5);
    EXPECT_EQ(pp.preprocessingType, Pose3dParam::PreprocessingType::HOMOGENEOUS_COORDINATES);
    EXPECT_EQ(pp.focalLength[0], 1469.2684222875848f);
    EXPECT_EQ(pp.focalLength[1], 1469.8823503910792f);
    EXPECT_EQ(pp.center[0], 650.1274669098675f);
    EXPECT_EQ(pp.center[1], 505.44118528424053f);
    EXPECT_EQ(pp.radialDistortion[0], -0.12686622768237987f);
    EXPECT_EQ(pp.radialDistortion[1], -0.33232941922945763f);
    EXPECT_EQ(pp.radialDistortion[2], 0.38770082f);
    EXPECT_EQ(pp.tanDistortion[0], 0.0015259532472412637f);
    EXPECT_EQ(pp.tanDistortion[1], 0.00295975558f);
    EXPECT_EQ(pp.pose2DJoints[0], 290);
    EXPECT_EQ(pp.pose2DJoints[1], 236);
}

TEST_F(Pose3dParamTest, 05_test_fromJson_partial)
{
    Pose3dParam pp;

    pp.fromJson("{ \"modelParam\": { \"maxBatchSize\": 3} }");
    EXPECT_EQ(pp.maxBatchSize, 3);

    pp.fromJson("{ \"modelParam\": { \"flipPoses\": false} }");
    EXPECT_EQ(pp.flipPoses, false);
    EXPECT_EQ(pp.maxBatchSize, 3);

}

TEST_F(Pose3dParamTest, 06_test_fromJson_parents)
{
    Pose3dParam pp;
    pp.fromJson("{ \
        \"modelParam\": { \
            \"maxBatchSize\": 4, \
            \"flipPoses\": true \
        } \
    }");

    EXPECT_EQ(pp.orgImgWidth, 640);
    EXPECT_EQ(pp.orgImgHeight, 480);
    EXPECT_EQ(pp.numMaxPerson, 1);
    EXPECT_EQ(pp.hasIntrinsics, true);
    EXPECT_EQ(pp.timeLogIntervalMs, 0);
}
