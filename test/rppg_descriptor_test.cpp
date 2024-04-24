/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/rppg/RppgDescriptor.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace aif;
namespace rj = rapidjson;

class RppgDescriptorTest : public ::testing::Test
{
protected:
    RppgDescriptorTest() = default;
    ~RppgDescriptorTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(RppgDescriptorTest, 01_rppg_constructor)
{
    RppgDescriptor descriptor;
    auto json = descriptor.toStr();

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
}

TEST_F(RppgDescriptorTest, 02_rppg_addBPM_test)
{
    RppgDescriptor descriptor;

    std::vector<float> rppgOutputs;
    for (int i = 1; i <= 200; i++)
    {
        rppgOutputs.push_back(static_cast<float>(i));
    }
    EXPECT_TRUE(rppgOutputs.size() == 200);
    descriptor.addRppgOutput(rppgOutputs);

    auto json = descriptor.toStr();
    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("rPPG"));
    EXPECT_TRUE(d["rPPG"].IsArray());
    EXPECT_TRUE(d["rPPG"].Size() == 1);

    EXPECT_TRUE(d["rPPG"][0].HasMember("outputTensors"));
    EXPECT_TRUE(d["rPPG"][0]["outputTensors"].IsArray());
    EXPECT_EQ(d["rPPG"][0]["outputTensors"].Size(), 200);
}
