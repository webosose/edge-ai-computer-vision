/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/personDetect/yolov3_yc/Yolov3YCParam.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>

using namespace aif;

class Yolov3YCParamTest : public ::testing::Test
{
protected:
    Yolov3YCParamTest() = default;
    ~Yolov3YCParamTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(Yolov3YCParamTest, 01_constructor)
{
    // default constructor
    Yolov3YCParam yp;
    yp.trace();

    Yolov3YCParam yp2;
    yp2.fromJson("{ \
        \"modelParam\": { \
            \"class_size\": 10, \
        \"lbbox_h\": 9 \
        } \
    }");
    yp2.trace();

    EXPECT_EQ(yp, yp2);
}

TEST_F(Yolov3YCParamTest, 02_copy_constructors)
{
    // constructor 1
    Yolov3YCParam yp1;
    yp1.trace();

    // copy constructor
    Yolov3YCParam yp2(yp1);
    EXPECT_EQ(yp1, yp2);

    // copy assignment operation
    Yolov3YCParam yp3;
    yp3 = yp1;
    ASSERT_EQ(yp3, yp1);
}

TEST_F(Yolov3YCParamTest, 03_move_constructors)
{
    // constructor 1
    Yolov3YCParam yp1;
    yp1.trace();

    // copy constructor
    Yolov3YCParam yp2(yp1);
    Yolov3YCParam yp3(yp1);

    // move constructor
    Yolov3YCParam yp4(std::move(yp2));

    EXPECT_EQ(yp4, yp1);

    // move assignment operation
    Yolov3YCParam yp5;
    yp5 = std::move(yp3);

    EXPECT_EQ(yp5, yp1);
}

TEST_F(Yolov3YCParamTest, 04_test_fromJson)
{
    Yolov3YCParam yp;

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


TEST_F(Yolov3YCParamTest, 05_test_fromJson_partial)
{
    Yolov3YCParam yp;

    yp.fromJson("{ \"modelParam\": { \"bboxBottomThresholdY\": -1} }");
    EXPECT_EQ(yp.orgImgWidth, 640);

    yp.fromJson("{ \"modelParam\": { \"useFp16\": false} }");
    EXPECT_EQ(yp.orgImgHeight, 480);

}

TEST_F(Yolov3YCParamTest, 06_test_fromJson_parents)
{
    Yolov3YCParam yp;
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
