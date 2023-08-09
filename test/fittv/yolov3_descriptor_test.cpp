/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/bodyPoseEstimation/personDetect/yolov3/Yolov3Descriptor.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace aif;
namespace rj = rapidjson;

class Yolov3DescriptorTest : public ::testing::Test
{
protected:
    Yolov3DescriptorTest() = default;
    ~Yolov3DescriptorTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(Yolov3DescriptorTest, 01_constructor)
{
    Yolov3Descriptor jfd;
    auto json = jfd.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
}

TEST_F(Yolov3DescriptorTest, 02_addperson_one)
{
    Yolov3Descriptor jfd;

    BBox bbox(640, 480);
    bbox.xmin = 2.0f;
    bbox.ymin = 3.0f;
    bbox.xmax = 4.0f;
    bbox.ymax = 5.0f;
    bbox.width = 2.0f;
    bbox.height = 2.0f;
    bbox.c0 = 3.0f;
    bbox.c1 = 4.0f;

    jfd.addPerson(80.0f, bbox);

    auto json = jfd.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("persons"));
    EXPECT_TRUE(d["persons"].IsArray());
    EXPECT_TRUE(floatEquals(d["persons"].Size(), 1));
    EXPECT_TRUE(d["persons"][0].IsObject());
    EXPECT_TRUE(d["persons"][0].HasMember("score"));
    EXPECT_TRUE(floatEquals(d["persons"][0]["score"].GetDouble(), 80.0));
    EXPECT_TRUE(d["persons"][0].HasMember("bbox"));
    EXPECT_TRUE(d["persons"][0]["bbox"].IsArray());
    EXPECT_TRUE(floatEquals(d["persons"][0]["bbox"].Size(), 6));
    EXPECT_TRUE(floatEquals(d["persons"][0]["bbox"][0].GetDouble(), 2.0));
    EXPECT_TRUE(floatEquals(d["persons"][0]["bbox"][1].GetDouble(), 3.0));
    EXPECT_TRUE(floatEquals(d["persons"][0]["bbox"][2].GetDouble(), 4.0));
    EXPECT_TRUE(floatEquals(d["persons"][0]["bbox"][3].GetDouble(), 5.0));
    EXPECT_TRUE(floatEquals(d["persons"][0]["bbox"][4].GetDouble(), 3.0));
    EXPECT_TRUE(floatEquals(d["persons"][0]["bbox"][5].GetDouble(), 4.0));
}

TEST_F(Yolov3DescriptorTest, 03_addperson_two)
{
    Yolov3Descriptor jfd;

    BBox bbox(640, 480);
    bbox.xmin = 2.0f;
    bbox.ymin = 3.0f;
    bbox.xmax = 4.0f;
    bbox.ymax = 5.0f;
    bbox.width = 2.0f;
    bbox.height = 2.0f;
    bbox.c_x = 3.0f;
    bbox.c_y = 4.0f;

    jfd.addPerson(80.0f, bbox);


    BBox bbox2(640, 480);
    bbox2.xmin = 12.0f;
    bbox2.ymin = 13.0f;
    bbox2.xmax = 14.0f;
    bbox2.ymax = 15.0f;
    bbox2.width = 12.0f;
    bbox2.height = 12.0f;
    bbox2.c_x = 13.0f;
    bbox2.c_y = 14.0f;

    jfd.addPerson(90.0f, bbox);

    auto json = jfd.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("persons"));
    EXPECT_TRUE(d["persons"].IsArray());
    EXPECT_TRUE(floatEquals(d["persons"].Size(), 2));
}

TEST_F(Yolov3DescriptorTest, 04_add_response_and_returncode)
{
    Yolov3Descriptor jfd;

    jfd.addResponseName("person_detect");
    jfd.addReturnCode(kAifOk);

    BBox bbox(640, 480);
    bbox.xmin = 2.0f;
    bbox.ymin = 3.0f;
    bbox.xmax = 4.0f;
    bbox.ymax = 5.0f;
    bbox.width = 2.0f;
    bbox.height = 2.0f;
    bbox.c_x = 3.0f;
    bbox.c_y = 4.0f;

    jfd.addPerson(80.0f, bbox);


    auto json = jfd.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("response"));
    EXPECT_TRUE(d.HasMember("returnCode"));
    EXPECT_TRUE(d.HasMember("persons"));
    EXPECT_TRUE(d["persons"].IsArray());
    EXPECT_TRUE(floatEquals(d["persons"].Size(), 1));
}

TEST_F(Yolov3DescriptorTest, 05_add_face)
{
    Yolov3Descriptor jfd;

    float score = 1.2;
    /*normalize 0~1 */
    float region_x = 1.0;
    float region_y = 0.7;
    float region_w = 0.5;
    float region_h = 0.0;
    jfd.addFace(score, region_x, region_y, region_w, region_h,
                0,0,0,0,0,0,0,0,0,0,0,0);

    auto json = jfd.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("faces"));
    EXPECT_TRUE(floatEquals(d["faces"].Size(), 1));
    EXPECT_TRUE(floatEquals(d["faces"][0]["score"].GetDouble(), 1.2));
    EXPECT_TRUE(floatEquals(d["faces"][0]["region"][0].GetDouble(), 1.0));
    EXPECT_TRUE(floatEquals(d["faces"][0]["region"][1].GetFloat(), 0.6999));
    EXPECT_TRUE(floatEquals(d["faces"][0]["region"][2].GetFloat(), 0.5));
    EXPECT_TRUE(floatEquals(d["faces"][0]["region"][3].GetDouble(), 0.0));
}


