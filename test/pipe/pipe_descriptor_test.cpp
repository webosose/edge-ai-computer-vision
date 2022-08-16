/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/PipeDescriptor.h>
#include <gtest/gtest.h>

using namespace std;
using namespace aif;

class PipeDescriptorTest: public ::testing::Test
{
protected:
    PipeDescriptorTest() = default;
    ~PipeDescriptorTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(PipeDescriptorTest, 01_type_check_setImage)
{
    PipeDescriptor nd;

    EXPECT_FALSE(nd.getType().isContain(NodeType::IMAGE));
    nd.setImage(cv::Mat());
    EXPECT_TRUE(nd.getType().isContain(NodeType::IMAGE));
    EXPECT_FALSE(nd.getType().isContain(NodeType::INFERENCE));
}

TEST_F(PipeDescriptorTest, 02_type_check_addBridgeOperationResult)
{
    PipeDescriptor nd;

    EXPECT_FALSE(nd.getType().isContain(NodeType::INFERENCE));
    nd.addBridgeOperationResult("test", R"({ "item1" : "test" })");
    EXPECT_TRUE(nd.getType().isContain(NodeType::INFERENCE));
    EXPECT_FALSE(nd.getType().isContain(NodeType::IMAGE));
}


TEST_F(PipeDescriptorTest, 03_type_check)
{
    PipeDescriptor nd;

    EXPECT_FALSE(nd.getType().isContain(NodeType::IMAGE));
    EXPECT_FALSE(nd.getType().isContain(NodeType::INFERENCE));

    nd.setImage(cv::Mat());
    nd.addBridgeOperationResult("test", R"({ "item1" : "test" })");

    EXPECT_TRUE(nd.getType().isContain(NodeType::INFERENCE));
    EXPECT_TRUE(nd.getType().isContain(NodeType::IMAGE));
}


TEST_F(PipeDescriptorTest, 04_addBridgeOperationResult)
{
    PipeDescriptor nd;
    nd.addBridgeOperationResult("test1", R"({ "item1" : "test" })");
    nd.addBridgeOperationResult("test2", R"({ "item2" : "test" })");
    nd.addBridgeOperationResult("test3", R"({ "item3" : "test" })");

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

TEST_F(PipeDescriptorTest, 05_getResult)
{
    std::string id1 = "test1";
    std::string id2 = "test2";
    std::string id3 = "test3";
    std::string result1 = R"({"item1":"test"})";
    std::string result2 = R"({"item2":"test"})";
    std::string result3 = R"({"item3":"test"})";

    std::string result =
        R"({"results":[{"test1":{"item1":"test"}},{"test2":{"item2":"test"}},{"test3":{"item3":"test"}}]})";

    PipeDescriptor nd;
    nd.addBridgeOperationResult(id1, result1);
    nd.addBridgeOperationResult(id2, result2);
    nd.addBridgeOperationResult(id3, result3);

    std::string ndResult = nd.getResult();
    EXPECT_EQ(result, ndResult);
}

TEST_F(PipeDescriptorTest, 05_getResultById)
{
    std::string id1 = "test1";
    std::string id2 = "test2";
    std::string id3 = "test3";
    std::string result1 = R"({"item1":"test"})";
    std::string result2 = R"({"item1":"test"})";
    std::string result3 = R"({"item1":"test"})";

    PipeDescriptor nd;
    nd.addBridgeOperationResult(id1, result1);
    nd.addBridgeOperationResult(id2, result2);
    nd.addBridgeOperationResult(id3, result3);

    std::string ndResult0 = nd.getResult("invalid_id");
    std::string ndResult1 = nd.getResult(id1);
    std::string ndResult2 = nd.getResult(id2);
    std::string ndResult3 = nd.getResult(id3);

    EXPECT_TRUE(ndResult0.empty());
    EXPECT_EQ(result1, ndResult1);
    EXPECT_EQ(result2, ndResult2);
    EXPECT_EQ(result3, ndResult3);
}
