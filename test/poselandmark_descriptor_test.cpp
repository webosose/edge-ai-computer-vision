#include <aif/poseLandmark/PoseLandmarkDescriptor.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;

namespace rj = rapidjson;

class PoseLandmarkDescriptorTest : public ::testing::Test
{
protected:
    PoseLandmarkDescriptorTest() = default;
    ~PoseLandmarkDescriptorTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

    int width{320};
    int height{240};
    void initMaskData(float* mask, bool reverse = false) {
        for (int i = 0; i < width * height; i++) {
            if (!reverse)
                mask[i] = (i%2 == 1) ? 1 : -10;
            else
                mask[i] = (i%2 == 0) ? 1 : -10;
        }
    }

};

TEST_F(PoseLandmarkDescriptorTest, 01_constructor)
{
    PoseLandmarkDescriptor descriptor;
    auto json = descriptor.toStr();

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
}

TEST_F(PoseLandmarkDescriptorTest, 02_addMaskData_one)
{
    float mask[width * height];
    initMaskData(mask);

    PoseLandmarkDescriptor descriptor;
    descriptor.addMaskData(width, height, mask);

    auto json = descriptor.toStr();
    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("segments"));
    EXPECT_TRUE(d["segments"].IsArray());
    EXPECT_TRUE(d["segments"].Size() == 1);
    EXPECT_TRUE(d["segments"][0].HasMember("width"));
    EXPECT_TRUE(d["segments"][0].HasMember("height"));
    EXPECT_TRUE(d["segments"][0].HasMember("mask"));
    EXPECT_EQ(d["segments"][0]["width"], width);
    EXPECT_EQ(d["segments"][0]["height"], height);
    EXPECT_TRUE(d["segments"][0]["mask"].IsArray());
    EXPECT_EQ(d["segments"][0]["mask"].Size(), width * height);
    int count = 0;
    for (int i = 0; i < width * height; i++) {
        if (d["segments"][0]["mask"][i].GetInt() == i%2) count++;
    }
    EXPECT_EQ(width * height, count);
}

TEST_F(PoseLandmarkDescriptorTest, 03_addMaskData_two)
{
    float mask1[width * height];
    float mask2[width * height];
    initMaskData(mask1, false);
    initMaskData(mask2, true);

    PoseLandmarkDescriptor descriptor;
    descriptor.addMaskData(width, height, mask1);
    descriptor.addMaskData(width, height, mask2);
    std::vector<std::vector<cv::Rect2f>> prev;
    descriptor.makeBodyParts(prev, 0.3f);

    auto json = descriptor.toStr();
    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("segments"));
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

TEST_F(PoseLandmarkDescriptorTest, 04_addLandmarks)
{
    PoseLandmarkDescriptor descriptor;
    std::vector<std::vector<float>> landmarks(PoseLandmarkDescriptor::NUM_LANDMARK_TYPES);
    for (int i = 0; i < PoseLandmarkDescriptor::NUM_LANDMARK_TYPES; i++) {
        for (int j = 0; j < PoseLandmarkDescriptor::NUM_LANDMARK_ITEMS; j++) {
            landmarks[i].emplace_back(i * 1.1);
        }
    }

    descriptor.addLandmarks(landmarks);
    std::vector<std::vector<cv::Rect2f>> prev;
    descriptor.makeBodyParts(prev, 0.3f);

    auto json = descriptor.toStr();
    rj::Document d;
    d.Parse(json.c_str());
    Logd(json);
    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("poses"));
    EXPECT_TRUE(d["poses"].IsArray());
    EXPECT_TRUE(d["poses"].Size() == 1);
    EXPECT_TRUE(d["poses"][0].IsObject());
    EXPECT_TRUE(d["poses"][0].HasMember("landmarks"));
    EXPECT_TRUE(d["poses"][0]["landmarks"].IsArray());
    EXPECT_EQ(PoseLandmarkDescriptor::NUM_LANDMARK_TYPES,
            d["poses"][0]["landmarks"].Size());

}

TEST_F(PoseLandmarkDescriptorTest, 05_add_response_and_returncode)
{
    float mask[width * height];
    initMaskData(mask, false);

    PoseLandmarkDescriptor descriptor;
    descriptor.addMaskData(width, height, mask);

    std::vector<std::vector<float>> landmarks(PoseLandmarkDescriptor::NUM_LANDMARK_TYPES);
    for (int i = 0; i < PoseLandmarkDescriptor::NUM_LANDMARK_TYPES; i++) {
        for (int j = 0; j < PoseLandmarkDescriptor::NUM_LANDMARK_ITEMS; j++) {
            landmarks[i].emplace_back(i * 1.1);
        }
    }

    descriptor.addLandmarks(landmarks);
    std::vector<std::vector<cv::Rect2f>> prev;
    descriptor.makeBodyParts(prev, 0.3f);

    descriptor.addResponseName("poselandmark_detect");
    descriptor.addReturnCode(kAifOk);

    auto json = descriptor.toStr();
    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("response"));
    EXPECT_TRUE(d.HasMember("returnCode"));
    EXPECT_TRUE(d.HasMember("segments"));
    EXPECT_TRUE(d["segments"].IsArray());
    EXPECT_TRUE(d["segments"].Size() == 1);
    EXPECT_TRUE(d.HasMember("poses"));
    EXPECT_TRUE(d["poses"].IsArray());
    EXPECT_TRUE(d["poses"].Size() == 1);
}
