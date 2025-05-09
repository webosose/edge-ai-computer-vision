/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/text/TextParam.h>

#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace aif;

class TextParamTest : public ::testing::Test
{
protected:
    TextParamTest() = default;
    ~TextParamTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(TextParamTest, 01_constructor)
{
    TextParam tp;
    tp.trace();

    TextParam tp2;
    tp2.trace();

    EXPECT_EQ(tp, tp2);
}

TEST_F(TextParamTest, 02_copy_constructors)
{
    // constructor 1
    TextParam tp1;
    tp1.trace();

    // copy constructor
    TextParam tp2(tp1);
    EXPECT_EQ(tp1, tp2);

    // copy assignment operation
    TextParam tp3;
    tp3 = tp1;
    ASSERT_EQ(tp3, tp1);
}

TEST_F(TextParamTest, 03_move_constructors)
{
    // constructor 1
    TextParam tp1;
    tp1.trace();

    // copy constructor
    TextParam tp2(tp1);
    TextParam tp3(tp1);

    // move constructor
    TextParam tp4(std::move(tp2));

    EXPECT_EQ(tp4, tp1);

    // move assignment operation
    TextParam tp5;
    tp5 = std::move(tp3);

    EXPECT_EQ(tp5, tp1);
}

TEST_F(TextParamTest, 04_test_fromJson)
{
    std::string param = R"(
            {
                "modelParam": {
                    "boxThreshold": 0.3,
                    "detectionRegion" : [100, 100, 720, 480]
                }
            })";
    TextParam tp;
    EXPECT_TRUE(floatEquals(tp.m_boxThreshold, 0.7f));
    EXPECT_FALSE(tp.m_useDetectionRegion);
    EXPECT_TRUE(tp.m_detectionRegion == cv::Rect(0, 0, 0, 0));

    tp.fromJson(param);
    EXPECT_TRUE(floatEquals(tp.m_boxThreshold, 0.3f));
    EXPECT_TRUE(tp.m_useDetectionRegion);
    EXPECT_TRUE(tp.m_detectionRegion == cv::Rect(100, 100, 720, 480));
}

TEST_F(TextParamTest, 05_test_fromJson_partial)
{
    TextParam tp;
    tp.fromJson(R"({ "modelParam": { "boxThreshold": 0.3 } })");
    EXPECT_TRUE(floatEquals(tp.m_boxThreshold, 0.3f));
    EXPECT_FALSE(tp.m_useDetectionRegion);

    TextParam tp1;
    tp1.fromJson(R"({ "modelParam": { "detectionRegion": [100, 100, 200, 200] } })");
    EXPECT_TRUE(floatEquals(tp1.m_boxThreshold, 0.7f));
    EXPECT_TRUE(tp1.m_useDetectionRegion);
    EXPECT_TRUE(tp1.m_detectionRegion == cv::Rect(100, 100, 200, 200));
}
