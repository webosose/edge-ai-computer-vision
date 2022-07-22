/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/handLandmark/HandLandmarkParam.h>

#include <gtest/gtest.h>

using namespace aif;

class HandLandmarkParamTest : public ::testing::Test
{
protected:
    HandLandmarkParamTest() = default;
    ~HandLandmarkParamTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(HandLandmarkParamTest, 01_copy_constructors)
{
    HandLandmarkParam param;
    param.trace();

    HandLandmarkParam param1(param); /* copy constructor */

    EXPECT_NE(param, param1);   /* compare addr. != */
}

TEST_F(HandLandmarkParamTest, 02_move_constructors)
{
    HandLandmarkParam param;
    param.trace();

    HandLandmarkParam param1(param); /* copy constructor */

    HandLandmarkParam param2(std::move(param1)); /* move constructor */

    EXPECT_NE(param2, param1);  /* compare addr. !=*/
}
