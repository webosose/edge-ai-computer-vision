/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/movenet/MovenetParam.h>

#include <gtest/gtest.h>

using namespace aif;

class MovenetParamTest : public ::testing::Test
{
protected:
    MovenetParamTest() = default;
    ~MovenetParamTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(MovenetParamTest, 01_copy_constructors)
{
    MovenetParam param;
    param.trace();

    MovenetParam param1(param); /* copy constructor */

    EXPECT_NE(param, param1);   /* compare addr. != */
}

TEST_F(MovenetParamTest, 02_move_constructors)
{
    MovenetParam param;
    param.trace();

    MovenetParam param1(param); /* copy constructor */

    MovenetParam param2(std::move(param1)); /* move constructor */

    EXPECT_NE(param2, param1);  /* compare addr. !=*/
}