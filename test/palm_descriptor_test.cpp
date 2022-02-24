#include <aif/palm/PalmDescriptor.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;
namespace rj = rapidjson;

class PalmDescriptorTest : public ::testing::Test
{
protected:
    PalmDescriptorTest() = default;
    ~PalmDescriptorTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(PalmDescriptorTest, 01_constructor)
{
    PalmDescriptor jfd;
    auto json = jfd.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
}

TEST_F(PalmDescriptorTest, 02_addpalm_one)
{
    PalmDescriptor jfd;
    float score = 1.0f;
    std::vector<float> keyPoints{ 
        1.0f, 2.0f, 3.0f, 4.0f, 5.0f,
        6.0f, 7.0f, 8.0f, 9.0f, 10.0f,
        11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f};

    jfd.addPalm(score, keyPoints);
    auto json = jfd.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("palms"));
    EXPECT_TRUE(d["palms"].IsArray());
    EXPECT_TRUE(d["palms"].Size() == 1);
    EXPECT_TRUE(d["palms"][0].IsObject());
    EXPECT_TRUE(d["palms"][0].HasMember("score"));
    EXPECT_TRUE(d["palms"][0]["score"].GetDouble() == 1.0);
    EXPECT_TRUE(d["palms"][0].HasMember("region"));
    EXPECT_TRUE(d["palms"][0]["region"].IsArray());
    EXPECT_TRUE(d["palms"][0]["region"].Size() == 4);
    EXPECT_TRUE(d["palms"][0]["region"][0].GetDouble() == 1.0);
    EXPECT_TRUE(d["palms"][0]["region"][1].GetDouble() == 2.0);
    EXPECT_TRUE(d["palms"][0]["region"][2].GetDouble() == 3.0);
    EXPECT_TRUE(d["palms"][0]["region"][3].GetDouble() == 4.0);
    EXPECT_TRUE(d["palms"][0].HasMember("keyPoints"));
    EXPECT_TRUE(d["palms"][0]["keyPoints"].IsArray());
    EXPECT_TRUE(d["palms"][0]["keyPoints"].Size() == 14);
    EXPECT_TRUE(d["palms"][0]["keyPoints"][0].GetDouble() == 5.0);
    EXPECT_TRUE(d["palms"][0]["keyPoints"][1].GetDouble() == 6.0);
    EXPECT_TRUE(d["palms"][0]["keyPoints"][2].GetDouble() == 7.0);
    EXPECT_TRUE(d["palms"][0]["keyPoints"][3].GetDouble() == 8.0);
    EXPECT_TRUE(d["palms"][0]["keyPoints"][4].GetDouble() == 9.0);
    EXPECT_TRUE(d["palms"][0]["keyPoints"][5].GetDouble() == 10.0);
    EXPECT_TRUE(d["palms"][0]["keyPoints"][6].GetDouble() == 11.0);
    EXPECT_TRUE(d["palms"][0]["keyPoints"][7].GetDouble() == 12.0);
    EXPECT_TRUE(d["palms"][0]["keyPoints"][8].GetDouble() == 13.0);
    EXPECT_TRUE(d["palms"][0]["keyPoints"][9].GetDouble() == 14.0);
    EXPECT_TRUE(d["palms"][0]["keyPoints"][10].GetDouble() == 15.0);
    EXPECT_TRUE(d["palms"][0]["keyPoints"][11].GetDouble() == 16.0);
    EXPECT_TRUE(d["palms"][0]["keyPoints"][12].GetDouble() == 17.0);
    EXPECT_TRUE(d["palms"][0]["keyPoints"][13].GetDouble() == 18.0);
}

TEST_F(PalmDescriptorTest, 03_addpalm_two)
{
    PalmDescriptor jfd;
    float score = 1.0f;
    std::vector<float> keyPoints{ 
        1.0f, 2.0f, 3.0f, 4.0f, 5.0f,
            6.0f, 7.0f, 8.0f, 9.0f, 10.0f,
            11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f};

    std::vector<float> keyPoints2{ 
        11.0f, 12.0f, 13.0f, 14.0f, 15.0f,
            16.0f, 17.0f, 18.0f, 19.0f, 20.0f,
            21.0f, 22.0f, 23.0f, 24.0f, 25.0f, 26.0f, 27.0f, 28.0f};

    jfd.addPalm(score, keyPoints);
    jfd.addPalm(score, keyPoints2);

    auto json = jfd.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("palms"));
    EXPECT_TRUE(d["palms"].IsArray());
    EXPECT_TRUE(d["palms"].Size() == 2);
    EXPECT_TRUE(d["palms"][0]["region"][0].GetDouble() == 1.0);
    EXPECT_TRUE(d["palms"][1]["region"][0].GetDouble() == 11.0);
}

TEST_F(PalmDescriptorTest, 04_add_response_and_returncode)
{
    PalmDescriptor jfd;

    jfd.addResponseName("palm_detect");
    jfd.addReturnCode(kAifOk);
    
    float score = 1.0f;
    std::vector<float> keyPoints{ 
        1.0f, 2.0f, 3.0f, 4.0f, 5.0f,
            6.0f, 7.0f, 8.0f, 9.0f, 10.0f,
            11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f};
    jfd.addPalm(score, keyPoints);

    auto json = jfd.toStr();
    Logd(json);

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("response"));
    EXPECT_TRUE(d.HasMember("returnCode"));
    EXPECT_TRUE(d.HasMember("palms"));
    EXPECT_TRUE(d["palms"].IsArray());
    EXPECT_TRUE(d["palms"].Size() == 1);
}
