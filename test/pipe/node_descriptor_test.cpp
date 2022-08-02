/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/NodeDescriptor.h>
#include <gtest/gtest.h>

using namespace std;
using namespace aif;

class NodeDescriptorTest: public ::testing::Test
{
protected:
    NodeDescriptorTest() = default;
    ~NodeDescriptorTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(NodeDescriptorTest, 01_type_check_setImage)
{
    NodeDescriptor nd;

    EXPECT_FALSE(nd.getType().isContain(NodeType::IMAGE));
    nd.setImage(cv::Mat());
    EXPECT_TRUE(nd.getType().isContain(NodeType::IMAGE));
    EXPECT_FALSE(nd.getType().isContain(NodeType::INFERENCE));
}

TEST_F(NodeDescriptorTest, 02_type_check_addOperationResult)
{
    NodeDescriptor nd;

    EXPECT_FALSE(nd.getType().isContain(NodeType::INFERENCE));
    nd.addOperationResult("test", R"({ "item1" : "test" })");
    EXPECT_TRUE(nd.getType().isContain(NodeType::INFERENCE));
    EXPECT_FALSE(nd.getType().isContain(NodeType::IMAGE));
}


TEST_F(NodeDescriptorTest, 03_type_check)
{
    NodeDescriptor nd;

    EXPECT_FALSE(nd.getType().isContain(NodeType::IMAGE));
    EXPECT_FALSE(nd.getType().isContain(NodeType::INFERENCE));

    nd.setImage(cv::Mat());
    nd.addOperationResult("test", R"({ "item1" : "test" })");

    EXPECT_TRUE(nd.getType().isContain(NodeType::INFERENCE));
    EXPECT_TRUE(nd.getType().isContain(NodeType::IMAGE));
}


TEST_F(NodeDescriptorTest, 04_append_get_result)
{
    NodeDescriptor nd;
    nd.addOperationResult("test1", R"({ "item1" : "test" })");
    nd.addOperationResult("test2", R"({ "item2" : "test" })");
    nd.addOperationResult("test3", R"({ "item3" : "test" })");

    std::string result = nd.getResult();

    rj::Document doc;
    doc.Parse(result.c_str());

    EXPECT_TRUE(doc.HasMember("results"));
    EXPECT_EQ(doc["results"].GetArray().Size(), 3);
    EXPECT_TRUE(doc["results"].GetArray()[0].HasMember("test1"));
    EXPECT_TRUE(doc["results"].GetArray()[0]["test1"].HasMember("item1"));
    EXPECT_TRUE(doc["results"].GetArray()[1].HasMember("test2"));
    EXPECT_TRUE(doc["results"].GetArray()[1]["test2"].HasMember("item2"));
    EXPECT_TRUE(doc["results"].GetArray()[2].HasMember("test3"));
    EXPECT_TRUE(doc["results"].GetArray()[2]["test3"].HasMember("item3"));
}
