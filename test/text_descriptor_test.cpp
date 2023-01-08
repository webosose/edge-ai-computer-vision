/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/text/TextDescriptor.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;
namespace rj = rapidjson;

class TextDescriptorTest : public ::testing::Test
{
protected:
    TextDescriptorTest() = default;
    ~TextDescriptorTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(TextDescriptorTest, 01_constructor)
{
    TextDescriptor td;
    auto json = td.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
}

TEST_F(TextDescriptorTest, 02_addTextRects)
{
    TextDescriptor td;
    td.addTextRects({
            {100, 100, 100, 100},
            {200, 200, 200, 200},
            {300, 300, 300, 300} });

    auto json = td.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("texts"));
    EXPECT_TRUE(d["texts"].IsArray());
    EXPECT_TRUE(d["texts"].Size() == 3);
    EXPECT_TRUE(d["texts"][0].IsArray());
    EXPECT_TRUE(d["texts"][0].Size() == 4);
    EXPECT_TRUE(d["texts"][0][0].GetInt() == 100);
    EXPECT_TRUE(d["texts"][0][1].GetInt() == 100);
    EXPECT_TRUE(d["texts"][0][2].GetInt() == 100);
    EXPECT_TRUE(d["texts"][0][3].GetInt() == 100);
    EXPECT_TRUE(d["texts"][1].IsArray());
    EXPECT_TRUE(d["texts"][1].Size() == 4);
    EXPECT_TRUE(d["texts"][1][0].GetInt() == 200);
    EXPECT_TRUE(d["texts"][1][1].GetInt() == 200);
    EXPECT_TRUE(d["texts"][1][2].GetInt() == 200);
    EXPECT_TRUE(d["texts"][1][3].GetInt() == 200);
    EXPECT_TRUE(d["texts"][2].IsArray());
    EXPECT_TRUE(d["texts"][2].Size() == 4);
    EXPECT_TRUE(d["texts"][2][0].GetInt() == 300);
    EXPECT_TRUE(d["texts"][2][1].GetInt() == 300);
    EXPECT_TRUE(d["texts"][2][2].GetInt() == 300);
    EXPECT_TRUE(d["texts"][2][3].GetInt() == 300);
}

TEST_F(TextDescriptorTest, 03_add_response_and_returncode)
{
    TextDescriptor td;

    td.addResponseName("text_detect");
    td.addReturnCode(kAifOk);
    td.addTextRects({
            {100, 100, 100, 100},
            {200, 200, 200, 200},
            {300, 300, 300, 300} });

    auto json = td.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("response"));
    EXPECT_TRUE(d.HasMember("returnCode"));
    EXPECT_TRUE(d.HasMember("texts"));
    EXPECT_TRUE(d["texts"].IsArray());
    EXPECT_TRUE(d["texts"].Size() == 3);
}

TEST_F(TextDescriptorTest, 04_empty_texts)
{
    TextDescriptor td;

    td.addResponseName("text_detect");
    td.addReturnCode(kAifOk);

    auto json = td.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());
    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("response"));
    EXPECT_TRUE(d.HasMember("returnCode"));
    EXPECT_TRUE(d.HasMember("texts"));
}
