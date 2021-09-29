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

TEST_F(PosenetDescriptorTest, jsonPosenetdesc01_constructor_test)
{
    PosenetDescriptor jpd;
    auto json = jpd.toStr();
    std::cout << json << std::endl;

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("poses"));
    EXPECT_TRUE(d["poses"].IsArray());
}

TEST_F(PosenetDescriptorTest, jsonPosenetdesc02_addKeyPoints_test)
{
    PosenetDescriptor jpd;

    std::vector<cv::Point> keypoints;
    for (int i = 0; i < 17; i++) {
        keypoints.emplace_back(cv::Point(1.1 * i, 1.1 * i));
    }
    jpd.addKeyPoints(keypoints);
    std::vector<std::vector<cv::Rect>> prev;
    jpd.makeBodyParts(prev);

    auto json = jpd.toStr();
    std::cout << json << std::endl;

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

TEST_F(PosenetDescriptorTest, jsonPosenetdesc03_addPosenet_two_test)
{
    PosenetDescriptor jpd;
    std::vector<cv::Point> keypoints1, keypoints2;
    for (int i = 0; i < 17; i++) {
        keypoints1.emplace_back(cv::Point(1.1 * i, 1.1 * i));
    }
    jpd.addKeyPoints(keypoints1);

    for (int i = 0; i < 17; i++) {
        keypoints2.emplace_back(cv::Point(2.1 * i, 2.1 * i));
    }
    jpd.addKeyPoints(keypoints2);
    std::vector<std::vector<cv::Rect>> prev;
    jpd.makeBodyParts(prev);

    auto json = jpd.toStr();
    std::cout << json << std::endl;

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("poses"));
    EXPECT_TRUE(d["poses"].IsArray());
    EXPECT_TRUE(d["poses"].Size() == 2);
}

TEST_F(PosenetDescriptorTest, jsonPosenetdesc04_add_response_and_returncode)
{
    PosenetDescriptor jpd;

    std::vector<cv::Point> keypoints;
    for (int i = 0; i < 17; i++) {
        keypoints.emplace_back(cv::Point(1.1 * i, 1.1 * i));
    }
    jpd.addKeyPoints(keypoints);
    std::vector<std::vector<cv::Rect>> prev;
    jpd.makeBodyParts(prev);

    jpd.addResponseName("face_detect");
    jpd.addReturnCode(kAifOk);

    auto json = jpd.toStr();
    std::cout << json << std::endl;

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("response"));
    EXPECT_TRUE(d.HasMember("returnCode"));
    EXPECT_TRUE(d.HasMember("poses"));
    EXPECT_TRUE(d["poses"].IsArray());
    EXPECT_TRUE(d["poses"].Size() == 1);
}
