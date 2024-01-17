/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/rppg/RppgParam.h>

#include <gtest/gtest.h>

using namespace aif;

class RppgParamTest : public ::testing::Test
{
protected:
    RppgParamTest() = default;
    ~RppgParamTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(RppgParamTest, 01_rppg_copy_constructors)
{
    RppgParam param1;
    param1.trace();

    RppgParam param2(param1);
    EXPECT_NE(param1, param2);
}

TEST_F(RppgParamTest, 02_rppg_move_constructors)
{
    RppgParam param1;
    param1.trace();

    RppgParam param2(param1);

    RppgParam param3(std::move(param2));
    EXPECT_NE(param3, param2);
}
