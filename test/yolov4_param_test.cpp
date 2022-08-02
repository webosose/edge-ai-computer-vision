/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/yolov4/Yolov4Param.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace aif;

class Yolov4ParamTest : public ::testing::Test
{
protected:
    Yolov4ParamTest() = default;
    ~Yolov4ParamTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(Yolov4ParamTest, 01_constructor)
{
    // default constructor
    Yolov4Param yp;
    yp.trace();

    Yolov4Param yp2;
    yp2.fromJson("{ \
        \"modelParam\": { \
            \"bboxBottomThresholdY\": -1, \
            \"useFp16\": false \
        } \
    }");
    yp2.trace();

    EXPECT_EQ(yp, yp2);
}

TEST_F(Yolov4ParamTest, 02_copy_constructors)
{
    // constructor 1
    Yolov4Param yp1;
    yp1.trace();

    // copy constructor
    Yolov4Param yp2(yp1);
    EXPECT_EQ(yp1, yp2);

    // copy assignment operation
    Yolov4Param yp3;
    yp3 = yp1;
    ASSERT_EQ(yp3, yp1);
}

TEST_F(Yolov4ParamTest, 03_move_constructors)
{
    // constructor 1
    Yolov4Param yp1;
    yp1.trace();

    // copy constructor
    Yolov4Param yp2(yp1);
    Yolov4Param yp3(yp1);

    // move constructor
    Yolov4Param yp4(std::move(yp2));

    EXPECT_EQ(yp4, yp1);

    // move assignment operation
    Yolov4Param yp5;
    yp5 = std::move(yp3);

    EXPECT_EQ(yp5, yp1);
}

TEST_F(Yolov4ParamTest, 04_test_fromJson)
{
    Yolov4Param yp;
    yp.fromJson("{ \
        \"modelParam\": { \
            \"bboxBottomThresholdY\": -1, \
            \"useFp16\": false, \
            \"nms_threshold\": 0.5, \
            \"bbox_conf_threshold\": 0.1,\
            \"numClasses\": 80,\
            \"numOutChannels\": 85,\
            \"strides\": [32, 16],\
            \"numAnchors\": [3, 3],\
            \"anchors\": [ 81,82, \
                           135,169, \
                           344,319, \
                           23,27, \
                           37,58, \
                           81,82 ] \
            } \
    }");
#if 0
            \"anchors\": [[[81,82], \
                           [135,169], \
                           [344,319]], \
                          [[23,27], \
                           [37,58], \
                           [81,82]] \
                         ],\

#endif
    EXPECT_EQ(yp.bboxBottomThresholdY, -1);
    EXPECT_EQ(yp.useFp16, false);
    EXPECT_EQ(yp.nms_threshold, 0.5);
    EXPECT_EQ(yp.bbox_conf_threshold, 0.1);
    EXPECT_EQ(yp.numClasses, 80);
    EXPECT_EQ(yp.numOutChannels, 85);
    EXPECT_EQ(yp.strides[0], 32);
    EXPECT_EQ(yp.strides[1], 16);
    EXPECT_EQ(yp.numAnchors[0], 3);
    EXPECT_EQ(yp.numAnchors[1], 3);

    EXPECT_EQ(yp.anchors[0][0].first, 81);
    EXPECT_EQ(yp.anchors[0][0].second, 82);
    EXPECT_EQ(yp.anchors[0][1].first, 135);
    EXPECT_EQ(yp.anchors[0][1].second, 169);
    EXPECT_EQ(yp.anchors[0][2].first, 344);
    EXPECT_EQ(yp.anchors[0][2].second, 319);

    EXPECT_EQ(yp.anchors[1][0].first, 23);
    EXPECT_EQ(yp.anchors[1][0].second, 27);
    EXPECT_EQ(yp.anchors[1][1].first, 37);
    EXPECT_EQ(yp.anchors[1][1].second, 58);
    EXPECT_EQ(yp.anchors[1][2].first, 81);
    EXPECT_EQ(yp.anchors[1][2].second, 82);
}

TEST_F(Yolov4ParamTest, 05_test_fromJson_partial)
{
    Yolov4Param yp;

    yp.fromJson("{ \"modelParam\": { \"bboxBottomThresholdY\": -1} }");
    EXPECT_EQ(yp.orgImgWidth, 640);

    yp.fromJson("{ \"modelParam\": { \"useFp16\": false} }");
    EXPECT_EQ(yp.orgImgHeight, 480);

}

TEST_F(Yolov4ParamTest, 06_test_fromJson_parents)
{
    Yolov4Param yp;
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
