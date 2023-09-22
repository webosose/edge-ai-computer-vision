/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/personDetect/yolov3_v2/Yolov3V2Param.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>

using namespace aif;

class Yolov3V2ParamTest : public ::testing::Test
{
protected:
    Yolov3V2ParamTest() = default;
    ~Yolov3V2ParamTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(Yolov3V2ParamTest, 01_constructor)
{
    // default constructor
    Yolov3V2Param yp;
    yp.trace();

    Yolov3V2Param yp2;
    yp2.fromJson("{ \
        \"modelParam\": { \
            \"class_size\": 10, \
        \"lbbox_h\": 9 \
        } \
    }");
    yp2.trace();

    EXPECT_EQ(yp, yp2);
}

TEST_F(Yolov3V2ParamTest, 02_copy_constructors)
{
    // constructor 1
    Yolov3V2Param yp1;
    yp1.trace();

    // copy constructor
    Yolov3V2Param yp2(yp1);
    EXPECT_EQ(yp1, yp2);

    // copy assignment operation
    Yolov3V2Param yp3;
    yp3 = yp1;
    ASSERT_EQ(yp3, yp1);
}

TEST_F(Yolov3V2ParamTest, 03_move_constructors)
{
    // constructor 1
    Yolov3V2Param yp1;
    yp1.trace();

    // copy constructor
    Yolov3V2Param yp2(yp1);
    Yolov3V2Param yp3(yp1);

    // move constructor
    Yolov3V2Param yp4(std::move(yp2));

    EXPECT_EQ(yp4, yp1);

    // move assignment operation
    Yolov3V2Param yp5;
    yp5 = std::move(yp3);

    EXPECT_EQ(yp5, yp1);
}

TEST_F(Yolov3V2ParamTest, 04_test_fromJson)
{
    Yolov3V2Param yp;

    yp.fromJson("{ \"modelParam\": { \
        \"class_size\": 10, \
        \"lbbox_h\": 9, \
        \"lbbox_w\": 15, \
        \"mbbox_h\": 18,\
        \"mbbox_w\": 30, \
        \"box_size\": 3, \
        \"stride\": [32, 16]} }"
    );

    EXPECT_EQ(yp.class_size, 10);
    EXPECT_EQ(yp.lbbox_h, 9);
    EXPECT_EQ(yp.lbbox_w, 15);
    EXPECT_EQ(yp.mbbox_h, 18);
    EXPECT_EQ(yp.mbbox_w, 30);
    EXPECT_EQ(yp.box_size, 3);
    EXPECT_EQ(yp.stride[0], 32);
    EXPECT_EQ(yp.stride[1], 16);
}


TEST_F(Yolov3V2ParamTest, 05_test_fromJson_partial)
{
    Yolov3V2Param yp;

    yp.fromJson("{ \"modelParam\": { \"bboxBottomThresholdY\": -1} }");
    EXPECT_EQ(yp.orgImgWidth, 640);

    yp.fromJson("{ \"modelParam\": { \"useFp16\": false} }");
    EXPECT_EQ(yp.orgImgHeight, 480);

}

TEST_F(Yolov3V2ParamTest, 06_test_fromJson_parents)
{
    Yolov3V2Param yp;
    yp.fromJson("{ \
        \"modelParam\": { \
            \"bboxBottomThresholdY\": -1, \
            \"useFp16\": false \
        } \
    }");

    EXPECT_EQ(yp.orgImgWidth, 640);
    EXPECT_EQ(yp.orgImgHeight, 480);
    EXPECT_EQ(yp.numMaxPerson, 1);
    EXPECT_EQ(yp.hasIntrinsics, true);
    EXPECT_EQ(yp.timeLogIntervalMs, 0);
}
