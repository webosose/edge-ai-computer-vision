/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bgSegment/BgSegmentParam.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>

using namespace aif;

class BgSegmentParamTest : public ::testing::Test
{
protected:
    BgSegmentParamTest() = default;
    ~BgSegmentParamTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(BgSegmentParamTest, 01_constructor)
{
    // default constructor
    BgSegmentParam yp;
    yp.trace();

    BgSegmentParam yp2;
    yp2.fromJson("{ \
        \"modelParam\": { \
            \"origImgRoiX\": 0, \
            \"origImgRoiY\": 0, \
            \"origImgRoiWidth\": 0, \
            \"origImgRoiHeight\": 0 \
        } \
    }");
    yp2.trace();

    EXPECT_EQ(yp, yp2);
}

TEST_F(BgSegmentParamTest, 02_copy_constructors)
{
    // constructor 1
    BgSegmentParam yp1;
    yp1.trace();

    // copy constructor
    BgSegmentParam yp2(yp1);
    EXPECT_EQ(yp1, yp2);

    // copy assignment operation
    BgSegmentParam yp3;
    yp3 = yp1;
    ASSERT_EQ(yp3, yp1);
}

TEST_F(BgSegmentParamTest, 03_move_constructors)
{
    // constructor 1
    BgSegmentParam yp1;
    yp1.trace();

    // copy constructor
    BgSegmentParam yp2(yp1);
    BgSegmentParam yp3(yp1);

    // move constructor
    BgSegmentParam yp4(std::move(yp2));

    EXPECT_EQ(yp4, yp1);

    // move assignment operation
    BgSegmentParam yp5;
    yp5 = std::move(yp3);

    EXPECT_EQ(yp5, yp1);
}

TEST_F(BgSegmentParamTest, 04_test_fromJson)
{
    BgSegmentParam yp;

    yp.fromJson("{ \"modelParam\": { \
            \"origImgRoiX\": 320, \
            \"origImgRoiY\": 0, \
            \"origImgRoiWidth\": 640, \
            \"origImgRoiHeight\": 720 \
        } }"
    );

    EXPECT_EQ(yp.origImgRoiX, 320);
    EXPECT_EQ(yp.origImgRoiY, 0);
    EXPECT_EQ(yp.origImgRoiWidth, 640);
    EXPECT_EQ(yp.origImgRoiHeight, 720);
}
