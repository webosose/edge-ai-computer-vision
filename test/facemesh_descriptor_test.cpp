/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/faceMesh/FaceMeshDescriptor.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <gtest/gtest.h>
#include <iostream>//
#include <fstream>//
#include <vector>

using namespace aif;

namespace rj = rapidjson;

class FaceMeshDescriptorTest : public ::testing::Test
{
protected:
    FaceMeshDescriptorTest() = default;
    ~FaceMeshDescriptorTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

    const int NUMBER_OF_LANDMARKS = 468;

    void initLandmarkData(std::vector<cv::Vec3f> &landmarks){
        for (int i = 0; i < NUMBER_OF_LANDMARKS; i++)
        {
            cv::Vec3f landmark = cv::Vec3f(0.1f, 0.3f, 0.5f);
            landmarks.push_back(landmark);
        }
    }
};

TEST_F(FaceMeshDescriptorTest, 01_constructor)
{
    FaceMeshDescriptor descriptor;
    auto json = descriptor.toStr();

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
}

TEST_F(FaceMeshDescriptorTest, 02_cpu_from_mona_face)
{
    FaceMeshDescriptor faceMeshDescriptor;

    std::vector<cv::Vec3f> landmarks;
    initLandmarkData(landmarks);
    faceMeshDescriptor.addLandmarks(landmarks, 1.0f);

    auto json = faceMeshDescriptor.toStr();
    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("facemesh"));
    EXPECT_TRUE(d["facemesh"].IsArray());
    EXPECT_TRUE(d["facemesh"].Size() == 1);

    EXPECT_TRUE(d["facemesh"][0].HasMember("landmarks"));
    EXPECT_TRUE(d["facemesh"][0]["landmarks"].IsArray()); // [x1, y1, z1, x2, y2, z2, ...]
    EXPECT_EQ(d["facemesh"][0]["landmarks"].Size() / 3, NUMBER_OF_LANDMARKS);

    EXPECT_TRUE(d["facemesh"][0].HasMember("score"));

    EXPECT_TRUE(d["facemesh"][0].HasMember("oval"));
    EXPECT_TRUE(d["facemesh"][0]["oval"].IsArray()); // [x1, y1, x2, y2, ...]
    EXPECT_EQ(d["facemesh"][0]["oval"].Size() / 2, sizeof(FACEMESH_OVAL) / sizeof(FACEMESH_OVAL[0]));

    EXPECT_TRUE(d["facemesh"][0].HasMember("leftEye"));
    EXPECT_TRUE(d["facemesh"][0]["leftEye"].IsArray()); // [x1, y1, x2, y2, ...]
    EXPECT_EQ(d["facemesh"][0]["leftEye"].Size() / 2, sizeof(FACEMESH_LEFTEYE) / sizeof(FACEMESH_LEFTEYE[0]));

    EXPECT_TRUE(d["facemesh"][0].HasMember("rightEye"));
    EXPECT_TRUE(d["facemesh"][0]["rightEye"].IsArray()); // [x1, y1, x2, y2, ...]
    EXPECT_EQ(d["facemesh"][0]["rightEye"].Size() / 2, sizeof(FACEMESH_RIGHTEYE) / sizeof(FACEMESH_RIGHTEYE[0]));

    EXPECT_TRUE(d["facemesh"][0].HasMember("mouth"));
    EXPECT_TRUE(d["facemesh"][0]["mouth"].IsArray()); // [x1, y1, x2, y2, ...]
    EXPECT_EQ(d["facemesh"][0]["mouth"].Size() / 2, sizeof(FACEMESH_MOUTH) / sizeof(FACEMESH_MOUTH[0]));
}
