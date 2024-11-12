/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/bodypix/BodypixDescriptor.h>

#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;
namespace rj = rapidjson;

class BodypixDescriptorTest : public ::testing::Test
{
protected:
    BodypixDescriptorTest() = default;
    ~BodypixDescriptorTest() = default;
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

    int width{320};
    int height{240};
    void initMaskData(uint8_t* mask, bool reverse = false) {
        for (int i = 0; i < width * height; i++) {
            if (!reverse) mask[i] = (i%2 == 1) ? 255 : 0;
            else mask[i] = (i%2 == 0) ? 255: 0;
        }
    }
};

TEST_F(BodypixDescriptorTest, 01_constructor)
{
    BodypixDescriptor descriptor;
    auto json = descriptor.toStr();

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
}

TEST_F(BodypixDescriptorTest, 02_addMaskData_one)
{
    uint8_t mask[width * height];
    initMaskData(mask);

    BodypixDescriptor descriptor;
    descriptor.addMaskData(width, height, mask);
    auto json = descriptor.toStr();
    rj::Document d;
    d.Parse(json.c_str());

    ASSERT_TRUE(d.IsObject());
    ASSERT_TRUE(d.HasMember("segments"));
    EXPECT_TRUE(d["segments"].IsArray());
    EXPECT_TRUE(d["segments"].Size() == 1);
    ASSERT_TRUE(d["segments"][0].HasMember("width"));
    ASSERT_TRUE(d["segments"][0].HasMember("height"));
    ASSERT_TRUE(d["segments"][0].HasMember("mask"));
    EXPECT_EQ(d["segments"][0]["width"], width);
    EXPECT_EQ(d["segments"][0]["height"], height);
    EXPECT_TRUE(d["segments"][0]["mask"].IsArray());
    EXPECT_EQ(d["segments"][0]["mask"].Size(), width * height);
    int count = 0;
    for (int i = 0; i < width * height; i++) {
        if (d["segments"][0]["mask"][i].GetInt() == i%2) count++;
    }
    EXPECT_EQ(count, width * height);
}

TEST_F(BodypixDescriptorTest, 03_addMaskData_two)
{
    uint8_t mask1[width * height];
    uint8_t mask2[width * height];
    initMaskData(mask1, false);
    initMaskData(mask2, true);

    BodypixDescriptor descriptor;
    descriptor.addMaskData(width, height, mask1);
    descriptor.addMaskData(width, height, mask2);

    auto json = descriptor.toStr();
    rj::Document d;
    d.Parse(json.c_str());

    ASSERT_TRUE(d.IsObject());
    ASSERT_TRUE(d.HasMember("segments"));
    EXPECT_TRUE(d["segments"].Size() == 2);
    int count1 = 0;
    int count2 = 0;
    for (int i = 0; i < width * height; i++) {
        if (d["segments"][0]["mask"][i].GetInt() == i%2) count1++;
        if (d["segments"][1]["mask"][i].GetInt() == ((i%2 == 0) ? 1:0)) count2++;
    }
    EXPECT_EQ(count1, width * height);
    EXPECT_EQ(count2, width * height);
}

TEST_F(BodypixDescriptorTest, 04_add_response_and_returncode)
{
    uint8_t mask[width * height];
    initMaskData(mask, false);

    BodypixDescriptor descriptor;
    descriptor.addMaskData(width, height, mask);
    descriptor.addResponseName("bodypix_detect");
    descriptor.addReturnCode(kAifOk);

    auto json = descriptor.toStr();
    rj::Document d;
    d.Parse(json.c_str());

    ASSERT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("response"));
    EXPECT_TRUE(d.HasMember("returnCode"));
    EXPECT_TRUE(d.HasMember("segments"));
}
