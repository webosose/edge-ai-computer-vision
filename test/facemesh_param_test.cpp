/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/faceMesh/FaceMeshParam.h>

#include <gtest/gtest.h>

using namespace aif;

class FaceMeshParamTest : public ::testing::Test
{
protected:
    FaceMeshParamTest() = default;
    ~FaceMeshParamTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(FaceMeshParamTest, 01_copy_constructors)
{
    FaceMeshParam param;
    param.trace();

    FaceMeshParam param1(param); /* copy constructor */

    EXPECT_NE(param, param1);   /* compare addr. != */
}

TEST_F(FaceMeshParamTest, 02_move_constructors)
{
    FaceMeshParam param;
    param.trace();

    FaceMeshParam param1(param); /* copy constructor */

    FaceMeshParam param2(std::move(param1)); /* move constructor */

    EXPECT_NE(param2, param1);  /* compare addr. !=*/
}
