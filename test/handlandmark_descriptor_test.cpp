/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aifex/handLandmark/HandLandmarkDescriptor.h>

#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;

namespace rj = rapidjson;

class HandLandmarkDescriptorTest : public ::testing::Test
{
protected:
    HandLandmarkDescriptorTest() = default;
    ~HandLandmarkDescriptorTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

};

TEST_F(HandLandmarkDescriptorTest, 01_constructor)
{
    HandLandmarkDescriptor descriptor;
    auto json = descriptor.toStr();

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
}

TEST_F(HandLandmarkDescriptorTest, 02_addLandmarks)
{
    HandLandmarkDescriptor descriptor;
    float landmarks[HandLandmarkDescriptor::LANDMARK_TYPE_COUNT * 
        HandLandmarkDescriptor::LANDMARK_ITEM_COUNT];
    for (int i = 0; i < HandLandmarkDescriptor::LANDMARK_TYPE_COUNT; i++) {
        for (int j = 0; j < HandLandmarkDescriptor::LANDMARK_ITEM_COUNT; j++) {
            landmarks[i * HandLandmarkDescriptor::LANDMARK_ITEM_COUNT + j] = i * 1.1;
        }
    }
    descriptor.addLandmarks(1.0, 1.0, landmarks, landmarks);
   
    auto json = descriptor.toStr();
    rj::Document d;
    d.Parse(json.c_str());
    Logd(json);
    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("hands"));
    EXPECT_TRUE(d["hands"].IsArray());
    EXPECT_TRUE(d["hands"].Size() == 1);
    EXPECT_TRUE(d["hands"][0].IsObject());
    EXPECT_TRUE(d["hands"][0].HasMember("landmarks"));
    EXPECT_TRUE(d["hands"][0].HasMember("landmarks3d"));
    EXPECT_TRUE(d["hands"][0]["landmarks"].IsArray());
    EXPECT_TRUE(d["hands"][0]["landmarks3d"].IsArray());
    EXPECT_EQ(HandLandmarkDescriptor::LANDMARK_TYPE_COUNT,
            d["hands"][0]["landmarks"].Size());

}

TEST_F(HandLandmarkDescriptorTest, 03_add_response_and_returncode)
{
    HandLandmarkDescriptor descriptor;
    float landmarks[HandLandmarkDescriptor::LANDMARK_TYPE_COUNT * 
        HandLandmarkDescriptor::LANDMARK_ITEM_COUNT];
    for (int i = 0; i < HandLandmarkDescriptor::LANDMARK_TYPE_COUNT; i++) {
        for (int j = 0; j < HandLandmarkDescriptor::LANDMARK_ITEM_COUNT; j++) {
            landmarks[i * HandLandmarkDescriptor::LANDMARK_ITEM_COUNT + j] = i * 1.1;
        }
    }

    descriptor.addLandmarks(1.0, 1.0, landmarks, landmarks);
    descriptor.addResponseName("handlandmark_detect");
    descriptor.addReturnCode(kAifOk);
   
    auto json = descriptor.toStr();
    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("response"));
    EXPECT_TRUE(d.HasMember("returnCode"));
    EXPECT_TRUE(d.HasMember("hands"));
    EXPECT_TRUE(d["hands"].IsArray());
    EXPECT_TRUE(d["hands"].Size() == 1);
}
