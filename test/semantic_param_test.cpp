/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/semantic/SemanticParam.h>

#include <gtest/gtest.h>

using namespace aif;

class SemanticParamTest : public ::testing::Test
{
protected:
    SemanticParamTest() = default;
    ~SemanticParamTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(SemanticParamTest, 01_copy_constructors)
{
    SemanticParam param;
    param.trace();

    SemanticParam param1(param); /* copy constructor */

    EXPECT_NE(param, param1);   /* compare addr. != */
}
