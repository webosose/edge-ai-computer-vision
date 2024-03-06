/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/signLanguageArea/SignLanguageAreaDescriptor.h>

#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;
namespace rj = rapidjson;

class SignLanguageAreaDescriptorTest : public ::testing::Test
{
protected:
    SignLanguageAreaDescriptorTest() = default;
    ~SignLanguageAreaDescriptorTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(SignLanguageAreaDescriptorTest, 01_constructor)
{
    SignLanguageAreaDescriptor td;
    auto json = td.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
}

TEST_F(SignLanguageAreaDescriptorTest, 02_addBox)
{
    SignLanguageAreaDescriptor td;
    td.addBoxes( {0.1f, 0.2f, 0.3f, 0.4f, 0.9f} );

    auto json = td.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    Logi("Output json: %s", json.c_str());
    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("signLanguageAreas"));
    EXPECT_TRUE(d["signLanguageAreas"].IsArray());
    EXPECT_TRUE(d["signLanguageAreas"].Size() == 5);
    EXPECT_TRUE(aif::floatEquals(d["signLanguageAreas"][0].GetFloat(), 0.1f));
    EXPECT_TRUE(aif::floatEquals(d["signLanguageAreas"][1].GetFloat(), 0.2f));
    EXPECT_TRUE(aif::floatEquals(d["signLanguageAreas"][2].GetFloat(), 0.3f));
    EXPECT_TRUE(aif::floatEquals(d["signLanguageAreas"][3].GetFloat(), 0.4f));
    EXPECT_TRUE(aif::floatEquals(d["signLanguageAreas"][4].GetFloat(), 0.9f));
}

TEST_F(SignLanguageAreaDescriptorTest, 03_addBox)
{
    SignLanguageAreaDescriptor td;
    td.addBoxes( {} );

    auto json = td.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("signLanguageAreas"));
    EXPECT_TRUE(d["signLanguageAreas"].IsArray());
    EXPECT_TRUE(d["signLanguageAreas"].Size() == 0);
}
