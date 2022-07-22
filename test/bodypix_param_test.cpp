/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodypix/BodypixParam.h>

#include <gtest/gtest.h>

using namespace aif;

class BodypixParamTest : public ::testing::Test
{
protected:
    BodypixParamTest() = default;
    ~BodypixParamTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(BodypixParamTest, 01_copy_constructors)
{
    BodypixParam param;
    param.trace();

    BodypixParam param1(param); /* copy constructor */

    EXPECT_NE(param, param1);   /* compare addr. != */
}

TEST_F(BodypixParamTest, 02_move_constructors)
{
    BodypixParam param;
    param.trace();

    BodypixParam param1(param); /* copy constructor */

    BodypixParam param2(std::move(param1)); /* move constructor */

    EXPECT_NE(param2, param1);  /* compare addr. !=*/
}
