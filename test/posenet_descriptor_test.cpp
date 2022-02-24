#include <aif/pose/PosenetDescriptor.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;

namespace rj = rapidjson;

class PosenetDescriptorTest : public ::testing::Test
{
protected:
    PosenetDescriptorTest() = default;
    ~PosenetDescriptorTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(PosenetDescriptorTest, 01_constructor)
{
    PosenetDescriptor jpd;
    auto json = jpd.toStr();

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
}

TEST_F(PosenetDescriptorTest, 02_addKeyPoints_one)
{
    PosenetDescriptor jpd;

    std::vector<cv::Point2f> keypoints;
    for (int i = 0; i < 17; i++) {
        keypoints.emplace_back(cv::Point2f(1.1 * i, 1.1 * i));
    }
    std::vector<float> scores(17);
    jpd.addKeyPoints(0.5, keypoints, scores);
    std::vector<std::vector<cv::Rect2f>> prev;
    jpd.makeBodyParts(prev);

    auto json = jpd.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("poses"));
    EXPECT_TRUE(d["poses"].IsArray());
    EXPECT_TRUE(d["poses"].Size() == 1);
    EXPECT_TRUE(d["poses"][0].IsObject());
    EXPECT_TRUE(d["poses"][0].HasMember("face"));
    EXPECT_TRUE(d["poses"][0].HasMember("body"));
    EXPECT_TRUE(d["poses"][0].HasMember("upperBody"));
}

TEST_F(PosenetDescriptorTest, 03_addPosenet_two)
{
    PosenetDescriptor jpd;
    std::vector<cv::Point2f> keypoints1, keypoints2;
    for (int i = 0; i < 17; i++) {
        keypoints1.emplace_back(cv::Point2f(1.1 * i, 1.1 * i));
    }
    std::vector<float> scores(17);
    jpd.addKeyPoints(0.5, keypoints1, scores);

    for (int i = 0; i < 17; i++) {
        keypoints2.emplace_back(cv::Point2f(2.1 * i, 2.1 * i));
    }
    jpd.addKeyPoints(0.5, keypoints2, scores);
    std::vector<std::vector<cv::Rect2f>> prev;
    jpd.makeBodyParts(prev);

    auto json = jpd.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("poses"));
    EXPECT_TRUE(d["poses"].IsArray());
    EXPECT_TRUE(d["poses"].Size() == 2);
}

TEST_F(PosenetDescriptorTest, 04_add_response_and_returncode)
{
    PosenetDescriptor jpd;

    std::vector<cv::Point2f> keypoints;
    for (int i = 0; i < 17; i++) {
        keypoints.emplace_back(cv::Point2f(1.1 * i, 1.1 * i));
    }
    std::vector<float> scores(17);
    jpd.addKeyPoints(0.5, keypoints, scores);
    std::vector<std::vector<cv::Rect2f>> prev;
    jpd.makeBodyParts(prev);

    jpd.addResponseName("face_detect");
    jpd.addReturnCode(kAifOk);

    auto json = jpd.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("response"));
    EXPECT_TRUE(d.HasMember("returnCode"));
    EXPECT_TRUE(d.HasMember("poses"));
    EXPECT_TRUE(d["poses"].IsArray());
    EXPECT_TRUE(d["poses"].Size() == 1);
}
