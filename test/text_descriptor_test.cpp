/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/text/TextDescriptor.h>

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

TEST_F(TextDescriptorTest, 02_addBbox)
{
    TextDescriptor td;
    td.addBboxes({
            {101, 102, 103, 104},
            {201, 202, 203, 204},
            {301, 302, 303, 304} });

    auto json = td.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("texts"));
    EXPECT_TRUE(d["texts"].HasMember("bbox"));
    EXPECT_TRUE(d["texts"]["bbox"].IsArray());
    EXPECT_TRUE(d["texts"]["bbox"].Size() == 3);
    EXPECT_TRUE(d["texts"]["bbox"][0].IsArray());
    EXPECT_TRUE(d["texts"]["bbox"][0].Size() == 4);
    EXPECT_TRUE(d["texts"]["bbox"][0][0].GetInt() == 101);
    EXPECT_TRUE(d["texts"]["bbox"][0][1].GetInt() == 102);
    EXPECT_TRUE(d["texts"]["bbox"][0][2].GetInt() == 103);
    EXPECT_TRUE(d["texts"]["bbox"][0][3].GetInt() == 104);
    EXPECT_TRUE(d["texts"]["bbox"][1].IsArray());
    EXPECT_TRUE(d["texts"]["bbox"][1].Size() == 4);
    EXPECT_TRUE(d["texts"]["bbox"][1][0].GetInt() == 201);
    EXPECT_TRUE(d["texts"]["bbox"][1][1].GetInt() == 202);
    EXPECT_TRUE(d["texts"]["bbox"][1][2].GetInt() == 203);
    EXPECT_TRUE(d["texts"]["bbox"][1][3].GetInt() == 204);
    EXPECT_TRUE(d["texts"]["bbox"][2].IsArray());
    EXPECT_TRUE(d["texts"]["bbox"][2].Size() == 4);
    EXPECT_TRUE(d["texts"]["bbox"][2][0].GetInt() == 301);
    EXPECT_TRUE(d["texts"]["bbox"][2][1].GetInt() == 302);
    EXPECT_TRUE(d["texts"]["bbox"][2][2].GetInt() == 303);
    EXPECT_TRUE(d["texts"]["bbox"][2][3].GetInt() == 304);
}

TEST_F(TextDescriptorTest, 03_addBox)
{
    TextDescriptor td;
    td.addBoxes({
            {{100, 200}, {300, 400}, {500, 600}, {700, 800}},
            {{101, 201}, {301, 401}, {501, 601}, {701, 801}},
            {{102, 202}, {302, 402}, {502, 602}, {702, 802}} });

    auto json = td.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("texts"));
    EXPECT_TRUE(d["texts"].HasMember("box"));
    EXPECT_TRUE(d["texts"]["box"].IsArray());
    EXPECT_TRUE(d["texts"]["box"].Size() == 3);

    EXPECT_TRUE(d["texts"]["box"][0].IsArray());
    EXPECT_TRUE(d["texts"]["box"][0].Size() == 8);
    EXPECT_TRUE(d["texts"]["box"][0][0].GetInt() == 100);
    EXPECT_TRUE(d["texts"]["box"][0][1].GetInt() == 200);
    EXPECT_TRUE(d["texts"]["box"][0][2].GetInt() == 300);
    EXPECT_TRUE(d["texts"]["box"][0][3].GetInt() == 400);
    EXPECT_TRUE(d["texts"]["box"][0][4].GetInt() == 500);
    EXPECT_TRUE(d["texts"]["box"][0][5].GetInt() == 600);
    EXPECT_TRUE(d["texts"]["box"][0][6].GetInt() == 700);
    EXPECT_TRUE(d["texts"]["box"][0][7].GetInt() == 800);

    EXPECT_TRUE(d["texts"]["box"][1].IsArray());
    EXPECT_TRUE(d["texts"]["box"][1].Size() == 8);
    EXPECT_TRUE(d["texts"]["box"][1][0].GetInt() == 101);
    EXPECT_TRUE(d["texts"]["box"][1][1].GetInt() == 201);
    EXPECT_TRUE(d["texts"]["box"][1][2].GetInt() == 301);
    EXPECT_TRUE(d["texts"]["box"][1][3].GetInt() == 401);
    EXPECT_TRUE(d["texts"]["box"][1][4].GetInt() == 501);
    EXPECT_TRUE(d["texts"]["box"][1][5].GetInt() == 601);
    EXPECT_TRUE(d["texts"]["box"][1][6].GetInt() == 701);
    EXPECT_TRUE(d["texts"]["box"][1][7].GetInt() == 801);
 
    EXPECT_TRUE(d["texts"]["box"][2].IsArray());
    EXPECT_TRUE(d["texts"]["box"][2].Size() == 8);
    EXPECT_TRUE(d["texts"]["box"][2][0].GetInt() == 102);
    EXPECT_TRUE(d["texts"]["box"][2][1].GetInt() == 202);
    EXPECT_TRUE(d["texts"]["box"][2][2].GetInt() == 302);
    EXPECT_TRUE(d["texts"]["box"][2][3].GetInt() == 402);
    EXPECT_TRUE(d["texts"]["box"][2][4].GetInt() == 502);
    EXPECT_TRUE(d["texts"]["box"][2][5].GetInt() == 602);
    EXPECT_TRUE(d["texts"]["box"][2][6].GetInt() == 702);
    EXPECT_TRUE(d["texts"]["box"][2][7].GetInt() == 802);
}

TEST_F(TextDescriptorTest, 04_add_response_and_returncode)
{
    TextDescriptor td;

    td.addResponseName("text_detect");
    td.addReturnCode(kAifOk);
    td.addBboxes({
            {101, 102, 103, 104},
            {201, 202, 203, 204},
            {301, 302, 303, 304} });
    td.addBoxes({
            {{100, 200}, {300, 400}, {500, 600}, {700, 800}},
            {{101, 201}, {301, 401}, {501, 601}, {701, 801}},
            {{102, 202}, {302, 402}, {502, 602}, {702, 802}} });


 
    auto json = td.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("response"));
    EXPECT_TRUE(d.HasMember("returnCode"));
    EXPECT_TRUE(d.HasMember("texts"));
    EXPECT_TRUE(d["texts"]["bbox"].IsArray());
    EXPECT_TRUE(d["texts"]["bbox"].Size() == 3);
    EXPECT_TRUE(d["texts"]["box"].IsArray());
    EXPECT_TRUE(d["texts"]["box"].Size() == 3);
}

TEST_F(TextDescriptorTest, 05_empty_texts)
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
