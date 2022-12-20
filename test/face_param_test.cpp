/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/face/FaceParam.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace aif;

class FaceParamTest : public ::testing::Test
{
protected:
    FaceParamTest() = default;
    ~FaceParamTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(FaceParamTest, 01_constructor)
{
    // default constructor
    FaceParam fp;
    fp.trace();

    FaceParam fp2;
    fp2.fromJson("{ \
        \"modelParam\": { \
            \"strides\": [8, 16, 16, 16], \
            \"optAspectRatios\": [1.0], \
            \"interpolatedScaleAspectRatio\": 1.0, \
            \"anchorOffsetX\": 0.5, \
            \"anchorOffsetY\": 0.5, \
            \"minScale\": 0.1484375, \
            \"maxScale\": 0.75, \
            \"reduceBoxesInLowestLayer\": false, \
            \"scoreThreshold\": 0.7, \
            \"iouThreshold\": 0.2, \
            \"maxOutputSize\": 100, \
            \"updateThreshold\": 0.3 \
        } \
    }");
    fp2.trace();

    EXPECT_EQ(fp, fp2);
}

TEST_F(FaceParamTest, 02_copy_constructors)
{
    // constructor 1
    FaceParam fp1;
    fp1.trace();

    // copy constructor
    FaceParam fp2(fp1);
    EXPECT_EQ(fp1, fp2);

    // copy assignment operation
    FaceParam fp3;
    fp3 = fp1;
    ASSERT_EQ(fp3, fp1);
}

TEST_F(FaceParamTest, 03_move_constructors)
{
    // constructor 1
    FaceParam fp1;
    fp1.trace();

    // copy constructor
    FaceParam fp2(fp1);
    FaceParam fp3(fp1);

    // move constructor
    FaceParam fp4(std::move(fp2));

    EXPECT_EQ(fp4, fp1);

    // move assignment operation
    FaceParam fp5;
    fp5 = std::move(fp3);

    EXPECT_EQ(fp5, fp1);
}

TEST_F(FaceParamTest, 04_test_fromJson)
{
    FaceParam fp;
    fp.fromJson("{ \
        \"modelParam\": { \
            \"strides\": [2, 4, 4, 2], \
            \"optAspectRatios\": [0.9], \
            \"interpolatedScaleAspectRatio\": 0.9, \
            \"anchorOffsetX\": 0.3, \
            \"anchorOffsetY\": 0.3, \
            \"minScale\": 0.14237, \
            \"maxScale\": 0.65, \
            \"reduceBoxesInLowestLayer\": true, \
            \"scoreThreshold\": 0.5, \
            \"iouThreshold\": 0.3, \
            \"maxOutputSize\": 90, \
            \"updateThreshold\": 0.2 \
        } \
    }");

    EXPECT_EQ(fp.strides.size(), 4);
    EXPECT_EQ(fp.strides[0], 2);
    EXPECT_EQ(fp.strides[1], 4);
    EXPECT_EQ(fp.strides[2], 4);
    EXPECT_EQ(fp.strides[3], 2);
    EXPECT_EQ(fp.optAspectRatios.size(), 1);
    EXPECT_EQ(fp.optAspectRatios[0], 0.9f);
    EXPECT_EQ(fp.interpolatedScaleAspectRatio, 0.9f);
    EXPECT_EQ(fp.anchorOffsetX, 0.3f);
    EXPECT_EQ(fp.anchorOffsetY, 0.3f);
    EXPECT_EQ(fp.minScale, 0.14237f);
    EXPECT_EQ(fp.maxScale, 0.65f);
    EXPECT_EQ(fp.reduceBoxesInLowestLayer, true);
    EXPECT_EQ(fp.scoreThreshold, 0.5f);
    EXPECT_EQ(fp.iouThreshold, 0.3f);
    EXPECT_EQ(fp.maxOutputSize, 90);
    EXPECT_EQ(fp.updateThreshold, 0.2f);

}

TEST_F(FaceParamTest, 05_test_fromJson_partial)
{
    FaceParam fp;
    fp.fromJson("{ \"modelParam\": { \"strides\": [4, 4]} } ");
    EXPECT_EQ(fp.strides.size(), 2);
    EXPECT_EQ(fp.strides[0], 4);
    EXPECT_EQ(fp.strides[1], 4);

    fp.fromJson("{ \"modelParam\": { \"optAspectRatios\": [0.9]} }");
    EXPECT_EQ(fp.optAspectRatios.size(), 1);
    EXPECT_EQ(fp.optAspectRatios[0], 0.9f);

    fp.fromJson("{ \"modelParam\": { \"interpolatedScaleAspectRatio\": 0.9} }");
    EXPECT_EQ(fp.interpolatedScaleAspectRatio, 0.9f);

    fp.fromJson("{ \"modelParam\": { \"anchorOffsetX\": 0.3} }");
    EXPECT_EQ(fp.anchorOffsetX, 0.3f);

    fp.fromJson("{ \"modelParam\": { \"anchorOffsetY\": 0.3} }");
    EXPECT_EQ(fp.anchorOffsetY, 0.3f);

    fp.fromJson("{ \"modelParam\": { \"minScale\": 0.14843} }");
    EXPECT_EQ(fp.minScale, 0.14843f);

    fp.fromJson("{ \"modelParam\": { \"maxScale\": 0.65} }");
    EXPECT_EQ(fp.maxScale, 0.65f);

    fp.fromJson("{ \"modelParam\": { \"reduceBoxesInLowestLayer\": true} }");
    EXPECT_EQ(fp.reduceBoxesInLowestLayer, true);

    fp.fromJson("{ \"modelParam\": { \"scoreThreshold\": 0.5} }");
    EXPECT_EQ(fp.scoreThreshold, 0.5f);

    fp.fromJson("{ \"modelParam\": { \"iouThreshold\": 0.3} }");
    EXPECT_EQ(fp.iouThreshold, 0.3f);

    fp.fromJson("{ \"modelParam\": { \"maxOutputSize\": 90} }");
    EXPECT_EQ(fp.maxOutputSize, 90);

    fp.fromJson("{ \"modelParam\": { \"updateThreshold\": 0.2} }");
    EXPECT_EQ(fp.updateThreshold, 0.2f);
}

