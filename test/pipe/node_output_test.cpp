/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/NodeInput.h>
#include <aif/pipe/NodeOutput.h>
#include <gtest/gtest.h>

using namespace std;
using namespace aif;

class NodeOutputTest: public ::testing::Test
{
protected:
    NodeOutputTest() = default;
    ~NodeOutputTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(NodeOutputTest, 01_verifyConnect)
{
    auto output1 = std::make_shared<NodeOutput>("o1", NodeType::IMAGE);
    auto output2 = std::make_shared<NodeOutput>("o2", NodeType::INFERENCE);
    auto output3 = std::make_shared<NodeOutput>("o3", NodeType::IMAGE|NodeType::INFERENCE);

    auto input1 = std::make_shared<NodeInput>("i1", NodeType::IMAGE);
    auto input2 = std::make_shared<NodeInput>("i2", NodeType::INFERENCE);
    auto input3 = std::make_shared<NodeInput>("i3", NodeType::IMAGE|NodeType::INFERENCE);

    EXPECT_TRUE(output1->verifyConnect(input1));
    EXPECT_FALSE(output1->verifyConnect(input2));
    EXPECT_FALSE(output1->verifyConnect(input3));

    EXPECT_FALSE(output2->verifyConnect(input1));
    EXPECT_TRUE(output2->verifyConnect(input2));
    EXPECT_FALSE(output2->verifyConnect(input3));

    EXPECT_TRUE(output3->verifyConnect(input1));
    EXPECT_TRUE(output3->verifyConnect(input2));
    EXPECT_TRUE(output3->verifyConnect(input3));

}

