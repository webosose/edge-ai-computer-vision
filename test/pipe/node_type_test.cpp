/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/NodeType.h>
#include <gtest/gtest.h>

using namespace std;
using namespace aif;

class NodeTypeTest: public ::testing::Test
{
protected:
    NodeTypeTest() = default;
    ~NodeTypeTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

    std::string strNone = "NONE";
    std::string strImage = "IMAGE";
    std::string strInference = "INFERENCE";

};

TEST_F(NodeTypeTest, 01_fromString)
{
    EXPECT_EQ(NodeType::NONE, NodeType::fromString(strNone));
    EXPECT_EQ(NodeType::NONE, NodeType::fromString("none"));

    EXPECT_EQ(NodeType::IMAGE, NodeType::fromString(strImage));
    EXPECT_EQ(NodeType::IMAGE, NodeType::fromString("image"));

    EXPECT_EQ(NodeType::INFERENCE, NodeType::fromString(strInference));
    EXPECT_EQ(NodeType::INFERENCE, NodeType::fromString("inference"));

}

TEST_F(NodeTypeTest, 02_isContain_int)
{
    int none = NodeType::NONE;
    int image = NodeType::IMAGE;
    int inference = NodeType::INFERENCE;

    NodeType type;
    EXPECT_TRUE(type.isContain(none));
    EXPECT_FALSE(type.isContain(image));
    EXPECT_FALSE(type.isContain(inference));
    EXPECT_FALSE(type.isContain(image|inference));

    NodeType typeImage(NodeType::IMAGE);
    EXPECT_TRUE(typeImage.isContain(none));
    EXPECT_TRUE(typeImage.isContain(image));
    EXPECT_FALSE(typeImage.isContain(inference));
    EXPECT_FALSE(typeImage.isContain(image|inference));

    NodeType typeInference(NodeType::INFERENCE);
    EXPECT_TRUE(typeInference.isContain(none));
    EXPECT_FALSE(typeInference.isContain(image));
    EXPECT_TRUE(typeInference.isContain(inference));
    EXPECT_FALSE(typeInference.isContain(image|inference));

    NodeType types(NodeType::IMAGE|NodeType::INFERENCE);
    EXPECT_TRUE(types.isContain(none));
    EXPECT_TRUE(types.isContain(image));
    EXPECT_TRUE(types.isContain(inference));
    EXPECT_TRUE(types.isContain(image|inference));
}

TEST_F(NodeTypeTest, 03_isContain_type)
{
    NodeType type;
    EXPECT_TRUE(type.isContain(NodeType::NONE));
    EXPECT_FALSE(type.isContain(NodeType::IMAGE));
    EXPECT_FALSE(type.isContain(NodeType::INFERENCE));

    NodeType typeImage(NodeType::IMAGE);
    EXPECT_TRUE(typeImage.isContain(NodeType::NONE));
    EXPECT_TRUE(typeImage.isContain(NodeType::IMAGE));
    EXPECT_FALSE(typeImage.isContain(NodeType::INFERENCE));

    NodeType typeInference(NodeType::INFERENCE);
    EXPECT_TRUE(typeInference.isContain(NodeType::NONE));
    EXPECT_FALSE(typeInference.isContain(NodeType::IMAGE));
    EXPECT_TRUE(typeInference.isContain(NodeType::INFERENCE));

    NodeType types(NodeType::IMAGE|NodeType::INFERENCE);
    EXPECT_TRUE(types.isContain(NodeType::NONE));
    EXPECT_TRUE(types.isContain(NodeType::IMAGE));
    EXPECT_TRUE(types.isContain(NodeType::INFERENCE));
}

TEST_F(NodeTypeTest, 04_addType)
{
    NodeType type;
    EXPECT_TRUE(type.isContain(NodeType::NONE));

    type.addType(NodeType::IMAGE);
    EXPECT_TRUE(type.isContain(NodeType::IMAGE));

    type.addType(NodeType::INFERENCE);
    EXPECT_TRUE(type.isContain(NodeType::INFERENCE));
    EXPECT_TRUE(type.isContain(NodeType::IMAGE|NodeType::INFERENCE));
}

TEST_F(NodeTypeTest, 05_toString)
{
    NodeType type;
    EXPECT_EQ(type.toString(), strNone);

    NodeType typeImage(NodeType::IMAGE);
    EXPECT_EQ(typeImage.toString(), strImage);

    NodeType typeInference(NodeType::INFERENCE);
    EXPECT_EQ(typeInference.toString(), strInference);

    NodeType types(NodeType::IMAGE|NodeType::INFERENCE);
    EXPECT_EQ(types.toString(), (strImage + "|" + strInference));
}

TEST_F(NodeTypeTest, 06_equal)
{
    NodeType none1(NodeType::NONE);
    NodeType image1(NodeType::IMAGE);
    NodeType inference1(NodeType::INFERENCE);
    NodeType types1(NodeType::IMAGE|NodeType::INFERENCE);

    NodeType none2(NodeType::NONE);
    NodeType image2(NodeType::IMAGE);
    NodeType inference2(NodeType::INFERENCE);
    NodeType types2(NodeType::IMAGE|NodeType::INFERENCE);

    EXPECT_EQ(none1, none2);
    EXPECT_EQ(image1, image1);
    EXPECT_EQ(inference1, inference2);
    EXPECT_EQ(types1, types2);

    EXPECT_NE(none1, image1);
    EXPECT_NE(none1, inference1);
    EXPECT_NE(none1, types1);

    EXPECT_NE(image1, inference1);
    EXPECT_NE(image1, types1);

    EXPECT_NE(inference1, types1);
}

