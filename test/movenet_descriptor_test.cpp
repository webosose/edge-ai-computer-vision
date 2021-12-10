#include <aif/movenet/MovenetDescriptor.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;

namespace rj = rapidjson;

class MovenetDescriptorTest : public ::testing::Test
{
protected:
    MovenetDescriptorTest() = default;
    ~MovenetDescriptorTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(MovenetDescriptorTest, 01_constructor)
{
    MovenetDescriptor descriptor;
    auto json = descriptor.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
}

TEST_F(MovenetDescriptorTest, 02_addKeyPoints_one)
{
    MovenetDescriptor descriptor;

    std::vector<cv::Point2f> keypoints;
    for (int i = 0; i < 17; i++) {
        keypoints.emplace_back(cv::Point2f(1.1 * i, 1.1 * i));
    }
    std::vector<float> scores(17);
    descriptor.addKeyPoints(keypoints, scores);
    std::vector<std::vector<cv::Rect2f>> prev;

    auto json = descriptor.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("poses"));
    EXPECT_TRUE(d["poses"].IsArray());
    EXPECT_TRUE(d["poses"][0].IsObject());
    EXPECT_TRUE(d["poses"][0].HasMember("keyPoints"));
    EXPECT_TRUE(d["poses"][0]["keyPoints"].IsArray());
}

TEST_F(MovenetDescriptorTest, 03_addKeyPoints_two)
{
    MovenetDescriptor descriptor;
    std::vector<cv::Point2f> keypoints1, keypoints2;
    for (int i = 0; i < 17; i++) {
        keypoints1.emplace_back(cv::Point2f(1.1 * i, 1.1 * i));
    }
    std::vector<float> scores(17);
    descriptor.addKeyPoints(keypoints1, scores);

    for (int i = 0; i < 17; i++) {
        keypoints2.emplace_back(cv::Point2f(2.1 * i, 2.1 * i));
    }
    descriptor.addKeyPoints(keypoints2, scores);
    std::vector<std::vector<cv::Rect2f>> prev;

    auto json = descriptor.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("poses"));
    EXPECT_TRUE(d["poses"].IsArray());
    EXPECT_TRUE(d["poses"].Size() == 2);
}

TEST_F(MovenetDescriptorTest, 04_add_response_and_returncode)
{
    MovenetDescriptor descriptor;

    std::vector<cv::Point2f> keypoints;
    for (int i = 0; i < 17; i++) {
        keypoints.emplace_back(cv::Point2f(1.1 * i, 1.1 * i));
    }
    std::vector<float> scores(17);
    descriptor.addKeyPoints(keypoints, scores);
    std::vector<std::vector<cv::Rect2f>> prev;

    descriptor.addResponseName("face_detect");
    descriptor.addReturnCode(kAifOk);

    auto json = descriptor.toStr();
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
