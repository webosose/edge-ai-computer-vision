/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/poseLandmark/PoseLandmarkParam.h>

#include <gtest/gtest.h>

using namespace aif;

class PoseLandmarkParamTest : public ::testing::Test
{
protected:
    PoseLandmarkParamTest() = default;
    ~PoseLandmarkParamTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(PoseLandmarkParamTest, 01_copy_constructors)
{
    PoseLandmarkParam param;
    param.trace();

    PoseLandmarkParam param1(param); /* copy constructor */

    EXPECT_NE(param, param1);   /* compare addr. != */
}

TEST_F(PoseLandmarkParamTest, 02_move_constructors)
{
    PoseLandmarkParam param;
    param.trace();

    PoseLandmarkParam param1(param); /* copy constructor */

    PoseLandmarkParam param2(std::move(param1)); /* move constructor */

    EXPECT_NE(param2, param1);  /* compare addr. !=*/
}