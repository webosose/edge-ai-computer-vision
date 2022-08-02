/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/NodeIO.h>
#include <gtest/gtest.h>

using namespace std;
using namespace aif;

class NodeIOTest: public ::testing::Test
{
protected:
    NodeIOTest() = default;
    ~NodeIOTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(NodeIOTest, 01_getType)
{
    NodeIO io("test", NodeType::NONE);
    EXPECT_EQ(io.getType(), NodeType::NONE);

    NodeIO io2("test", NodeType::IMAGE);
    EXPECT_EQ(io2.getType(), NodeType::IMAGE);

    NodeIO io3("test", NodeType::INFERENCE);
    EXPECT_EQ(io3.getType(), NodeType::INFERENCE);

    NodeType type(NodeType::IMAGE|NodeType::INFERENCE);
    NodeIO io4("test", type);
    EXPECT_EQ(io4.getType(), type);
}

TEST_F(NodeIOTest, 02_getId)
{
    std::string id = "testid";
    NodeIO io(id, NodeType::NONE);
    EXPECT_EQ(io.getId(), id);
}

TEST_F(NodeIOTest, 03_verifyDescriptor_type_none)
{
    std::shared_ptr<NodeDescriptor> d0 =
        std::make_shared<NodeDescriptor>();
    std::shared_ptr<NodeDescriptor> d1 =
        std::make_shared<NodeDescriptor>();
    d1->setImage(cv::Mat());
    std::shared_ptr<NodeDescriptor> d2 =
        std::make_shared<NodeDescriptor>();
    d2->addOperationResult("test", R"({"test":"test"})");
    std::shared_ptr<NodeDescriptor> d3 =
        std::make_shared<NodeDescriptor>();
    d3->setImage(cv::Mat());
    d3->addOperationResult("test", R"({"test":"test"})");

    NodeIO io("test", NodeType::NONE);
    EXPECT_TRUE(io.setDescriptor(d0));
    EXPECT_TRUE(io.setDescriptor(d1));
    EXPECT_TRUE(io.setDescriptor(d2));
    EXPECT_TRUE(io.setDescriptor(d3));
}

TEST_F(NodeIOTest, 04_verifyDescriptor_type_image)
{
    std::shared_ptr<NodeDescriptor> d0 =
        std::make_shared<NodeDescriptor>();
    std::shared_ptr<NodeDescriptor> d1 =
        std::make_shared<NodeDescriptor>();
    d1->setImage(cv::Mat());
    std::shared_ptr<NodeDescriptor> d2 =
        std::make_shared<NodeDescriptor>();
    d2->addOperationResult("test", R"({"test":"test"})");
    std::shared_ptr<NodeDescriptor> d3 =
        std::make_shared<NodeDescriptor>();
    d3->setImage(cv::Mat());
    d3->addOperationResult("test", R"({"test":"test"})");

    NodeIO io("test", NodeType::IMAGE);
    EXPECT_FALSE(io.setDescriptor(d0));
    EXPECT_TRUE(io.setDescriptor(d1));
    EXPECT_FALSE(io.setDescriptor(d2));
    EXPECT_TRUE(io.setDescriptor(d3));
}

TEST_F(NodeIOTest, 05_verifyDescriptor_type_inference)
{
    std::shared_ptr<NodeDescriptor> d0 =
        std::make_shared<NodeDescriptor>();
     std::shared_ptr<NodeDescriptor> d1 =
        std::make_shared<NodeDescriptor>();
    d1->setImage(cv::Mat());
    std::shared_ptr<NodeDescriptor> d2 =
        std::make_shared<NodeDescriptor>();
    d2->addOperationResult("test", R"({"test":"test"})");
    std::shared_ptr<NodeDescriptor> d3 =
        std::make_shared<NodeDescriptor>();
    d3->setImage(cv::Mat());
    d3->addOperationResult("test", R"({"test":"test"})");

    NodeIO io("test", NodeType::INFERENCE);
    EXPECT_FALSE(io.setDescriptor(d0));
    EXPECT_FALSE(io.setDescriptor(d1));
    EXPECT_TRUE(io.setDescriptor(d2));
    EXPECT_TRUE(io.setDescriptor(d3));
}

TEST_F(NodeIOTest, 06_verifyDescriptor_types)
{
    std::shared_ptr<NodeDescriptor> d0 =
        std::make_shared<NodeDescriptor>();
    std::shared_ptr<NodeDescriptor> d1 =
        std::make_shared<NodeDescriptor>();
    d1->setImage(cv::Mat());
    std::shared_ptr<NodeDescriptor> d2 =
        std::make_shared<NodeDescriptor>();
    d2->addOperationResult("test", R"({"test":"test"})");
    std::shared_ptr<NodeDescriptor> d3 =
        std::make_shared<NodeDescriptor>();
    d3->setImage(cv::Mat());
    d3->addOperationResult("test", R"({"test":"test"})");

    NodeIO io("test", NodeType::IMAGE|NodeType::INFERENCE);
    EXPECT_FALSE(io.setDescriptor(d0));
    EXPECT_FALSE(io.setDescriptor(d1));
    EXPECT_FALSE(io.setDescriptor(d2));
    EXPECT_TRUE(io.setDescriptor(d3));
}

TEST_F(NodeIOTest, 07_getDescriptor)
{
    std::shared_ptr<NodeDescriptor> d =
        std::make_shared<NodeDescriptor>();
    d->setImage(cv::Mat());
    d->addOperationResult("test", R"({"test":"test"})");

    NodeIO io("test", NodeType::IMAGE|NodeType::INFERENCE);
    EXPECT_EQ(io.getDescriptor().get(), nullptr);
    EXPECT_TRUE(io.setDescriptor(d));
    EXPECT_EQ(io.getDescriptor().get(), d.get());
}
