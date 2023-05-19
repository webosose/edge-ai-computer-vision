/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/BodyPoseEstimationParam.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <aif/face/FaceParam.h>
using namespace aif;

class BodyPoseEstimationParamTest : public ::testing::Test
{
protected:
    BodyPoseEstimationParamTest() = default;
    ~BodyPoseEstimationParamTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(BodyPoseEstimationParamTest, 01_constructor)
{
    // default constructor
    BodyPoseEstimationParam bp;
    bp.trace();

    BodyPoseEstimationParam bp2;
    bp2.fromJson("{ \
        \"commonParam\": { \
            \"orgImgWidth\": 640, \
            \"orgImgHeight\": 480, \
            \"numMaxPerson\": 1, \
            \"hasIntrinsics\": true, \
            \"timeLogIntervalsMs\": 0 \
        } \
    }");
    bp2.trace();

    EXPECT_EQ(bp, bp2);
}

TEST_F(BodyPoseEstimationParamTest, 02_copy_constructors)
{
    // constructor 1
    BodyPoseEstimationParam bp1;
    bp1.trace();

    // copy constructor
    BodyPoseEstimationParam bp2(bp1);
    EXPECT_EQ(bp1, bp2);

    // copy assignment operation
    BodyPoseEstimationParam bp3;
    bp3 = bp1;
    ASSERT_EQ(bp3, bp1);
}

TEST_F(BodyPoseEstimationParamTest, 03_move_constructors)
{
    // constructor 1
    BodyPoseEstimationParam bp1;
    bp1.trace();

    // copy constructor
    BodyPoseEstimationParam bp2(bp1);
    BodyPoseEstimationParam bp3(bp1);

    // move constructor
    BodyPoseEstimationParam bp4(std::move(bp2));

    EXPECT_EQ(bp4, bp1);

    // move assignment operation
    BodyPoseEstimationParam bp5;
    bp5 = std::move(bp3);

    EXPECT_EQ(bp5, bp1);
}

TEST_F(BodyPoseEstimationParamTest, 04_test_fromJson)
{
    BodyPoseEstimationParam bp;
    bp.fromJson("{ \
        \"commonParam\": { \
            \"orgImgWidth\": 640, \
            \"orgImgHeight\": 480, \
            \"numMaxPerson\": 1, \
            \"hasIntrinsics\": true, \
            \"timeLogIntervalMs\": 0 \
        } \
    }");

    EXPECT_EQ(bp.orgImgWidth, 640);
    EXPECT_EQ(bp.orgImgHeight, 480);
    EXPECT_EQ(bp.numMaxPerson, 1);
    EXPECT_EQ(bp.hasIntrinsics, true);
    EXPECT_EQ(bp.timeLogIntervalMs, 0);
}

TEST_F(BodyPoseEstimationParamTest, 05_test_fromJson_partial)
{
    BodyPoseEstimationParam bp;

    bp.fromJson("{ \"commonParam\": { \"orgImgWidth\": 640} }");
    EXPECT_EQ(bp.orgImgWidth, 640);

    bp.fromJson("{ \"commonParam\": { \"orgImgHeight\": 480} }");
    EXPECT_EQ(bp.orgImgHeight, 480);

    bp.fromJson("{ \"commonParam\": { \"numMaxPerson\": 1} }");
    EXPECT_EQ(bp.numMaxPerson, 1);

    bp.fromJson("{ \"commonParam\": { \"hasIntrinsics\": true} }");
    EXPECT_EQ(bp.hasIntrinsics, true);

    bp.fromJson("{ \"commonParam\": { \"timeLogIntervalMs\": 0} }");
    EXPECT_EQ(bp.timeLogIntervalMs, 0);

}

