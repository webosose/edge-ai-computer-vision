/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/pose/PosenetParam.h>

#include <gtest/gtest.h>

using namespace aif;

class PosenetParamTest : public ::testing::Test
{
protected:
    PosenetParamTest() = default;
    ~PosenetParamTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(PosenetParamTest, 01_copy_constructors)
{
    PosenetParam param;
    param.trace();

    PosenetParam param1(param); /* copy constructor */

    EXPECT_NE(param, param1);   /* compare addr. != */
}
