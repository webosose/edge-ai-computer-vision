/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/face/YuNetFaceParam.h>

#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace aif;

class YuNetFaceParamTest : public ::testing::Test
{
protected:
    YuNetFaceParamTest() = default;
    ~YuNetFaceParamTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(YuNetFaceParamTest, 01_constructor)
{
    YuNetFaceParam fp;
    fp.trace();

    YuNetFaceParam fp2;
    fp2.fromJson("{ \
        \"modelParam\": { \
            \"scoreThreshold\": 0.7, \
            \"nmsThreshold\": 0.3, \
            \"topK\": 5000 \
        } \
    }");
    fp2.trace();

    EXPECT_EQ(fp, fp2);
}

TEST_F(YuNetFaceParamTest, 02_copy_constructors)
{
    // constructor 1
    YuNetFaceParam fp1;
    fp1.trace();

    // copy constructor
    YuNetFaceParam fp2(fp1);
    EXPECT_EQ(fp1, fp2);

    // copy assignment operation
    YuNetFaceParam fp3;
    fp3 = fp1;
    ASSERT_EQ(fp3, fp1);
}

TEST_F(YuNetFaceParamTest, 03_move_constructors)
{
    // constructor 1
    YuNetFaceParam fp1;
    fp1.trace();

    // copy constructor
    YuNetFaceParam fp2(fp1);
    YuNetFaceParam fp3(fp1);

    // move constructor
    YuNetFaceParam fp4(std::move(fp2));

    EXPECT_EQ(fp4, fp1);

    // move assignment operation
    YuNetFaceParam fp5;
    fp5 = std::move(fp3);

    EXPECT_EQ(fp5, fp1);
}

TEST_F(YuNetFaceParamTest, 04_test_fromJson)
{
    YuNetFaceParam fp;
    fp.fromJson("{ \
        \"modelParam\": { \
            \"scoreThreshold\": 0.9, \
            \"nmsThreshold\": 0.5, \
            \"topK\": 1000 \
        } \
    }");

    EXPECT_EQ(fp.scoreThreshold, 0.9f);
    EXPECT_EQ(fp.nmsThreshold, 0.5f);
    EXPECT_EQ(fp.topK, 1000);

}

TEST_F(YuNetFaceParamTest, 05_test_fromJson_partial)
{
    YuNetFaceParam fp;
    fp.fromJson("{ \"modelParam\": { \"scoreThreshold\": 0.5} }");
    EXPECT_EQ(fp.scoreThreshold, 0.5f);

    fp.fromJson("{ \"modelParam\": { \"nmsThreshold\": 0.2} }");
    EXPECT_EQ(fp.nmsThreshold, 0.2f);

    fp.fromJson("{ \"modelParam\": { \"topK\": 1000} }");
    EXPECT_EQ(fp.topK, 1000);
}

TEST_F(YuNetFaceParamTest, 04_test_updateParam)
{
    YuNetFaceParam fp;
    fp.fromJson("{ \
        \"modelParam\": { \
            \"scoreThreshold\": 0.9, \
            \"nmsThreshold\": 0.5, \
            \"topK\": 1000 \
        } \
    }");

    EXPECT_EQ(fp.scoreThreshold, 0.9f);
    EXPECT_EQ(fp.nmsThreshold, 0.5f);
    EXPECT_EQ(fp.topK, 1000);

    fp.updateParam("{ \
        \"modelParam\": { \
            \"scoreThreshold\": 0.6, \
            \"nmsThreshold\": 0.5, \
            \"topK\": 100 \
        } \
    }");

    EXPECT_EQ(fp.scoreThreshold, 0.6f);
    EXPECT_EQ(fp.nmsThreshold, 0.5f);
    EXPECT_EQ(fp.topK, 100);
}


