/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/selfie/SelfieParam.h>

#include <gtest/gtest.h>

using namespace aif;

class SelfieParamTest : public ::testing::Test
{
protected:
    SelfieParamTest() = default;
    ~SelfieParamTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(SelfieParamTest, 01_copy_constructors)
{
    SelfieParam param;
    param.trace();

    SelfieParam param1(param); /* copy constructor */

    EXPECT_NE(param, param1);   /* compare addr. != */
}
