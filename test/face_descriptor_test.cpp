#include <aif/face/FaceDescriptor.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using namespace aif;
namespace rj = rapidjson;

class FaceDescriptorTest : public ::testing::Test
{
protected:
    FaceDescriptorTest() = default;
    ~FaceDescriptorTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(FaceDescriptorTest, jsonfacedesc01_constructor_test)
{
    FaceDescriptor jfd;
    auto json = jfd.toStr();
    std::cout << json << std::endl;

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("faces"));
    EXPECT_TRUE(d["faces"].IsArray());
}

TEST_F(FaceDescriptorTest, jsonfacedesc02_addface_one_test)
{
    FaceDescriptor jfd;

    // 17 elements
    jfd.addFace(1.0f, 2.0f, 3.0f, 4.0f, 5.0f,
                6.0f, 7.0f, 8.0f, 9.0f, 10.0f,
                11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f);

    auto json = jfd.toStr();
    std::cout << json << std::endl;

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("faces"));
    EXPECT_TRUE(d["faces"].IsArray());
    EXPECT_TRUE(d["faces"].Size() == 1);
    EXPECT_TRUE(d["faces"][0].IsObject());
    EXPECT_TRUE(d["faces"][0].HasMember("score"));
    EXPECT_TRUE(d["faces"][0]["score"].GetDouble() == 1.0);
    EXPECT_TRUE(d["faces"][0].HasMember("region"));
    EXPECT_TRUE(d["faces"][0]["region"].IsArray());
    EXPECT_TRUE(d["faces"][0]["region"].Size() == 4);
    EXPECT_TRUE(d["faces"][0]["region"][0].GetDouble() == 2.0);
    EXPECT_TRUE(d["faces"][0]["region"][1].GetDouble() == 3.0);
    EXPECT_TRUE(d["faces"][0]["region"][2].GetDouble() == 4.0);
    EXPECT_TRUE(d["faces"][0]["region"][3].GetDouble() == 5.0);
    EXPECT_TRUE(d["faces"][0].HasMember("lefteye"));
    EXPECT_TRUE(d["faces"][0]["lefteye"].IsArray());
    EXPECT_TRUE(d["faces"][0]["lefteye"].Size() == 2);
    EXPECT_TRUE(d["faces"][0]["lefteye"][0].GetDouble() == 6.0);
    EXPECT_TRUE(d["faces"][0]["lefteye"][1].GetDouble() == 7.0);
    EXPECT_TRUE(d["faces"][0].HasMember("righteye"));
    EXPECT_TRUE(d["faces"][0]["righteye"].IsArray());
    EXPECT_TRUE(d["faces"][0]["righteye"].Size() == 2);
    EXPECT_TRUE(d["faces"][0]["righteye"][0].GetDouble() == 8.0);
    EXPECT_TRUE(d["faces"][0]["righteye"][1].GetDouble() == 9.0);
    EXPECT_TRUE(d["faces"][0].HasMember("nosetip"));
    EXPECT_TRUE(d["faces"][0]["nosetip"].IsArray());
    EXPECT_TRUE(d["faces"][0]["nosetip"].Size() == 2);
    EXPECT_TRUE(d["faces"][0]["nosetip"][0].GetDouble() == 10.0);
    EXPECT_TRUE(d["faces"][0]["nosetip"][1].GetDouble() == 11.0);
    EXPECT_TRUE(d["faces"][0].HasMember("mouth"));
    EXPECT_TRUE(d["faces"][0]["mouth"].IsArray());
    EXPECT_TRUE(d["faces"][0]["mouth"].Size() == 2);
    EXPECT_TRUE(d["faces"][0]["mouth"][0].GetDouble() == 12.0);
    EXPECT_TRUE(d["faces"][0]["mouth"][1].GetDouble() == 13.0);
    EXPECT_TRUE(d["faces"][0].HasMember("leftear"));
    EXPECT_TRUE(d["faces"][0]["leftear"].IsArray());
    EXPECT_TRUE(d["faces"][0]["leftear"].Size() == 2);
    EXPECT_TRUE(d["faces"][0]["leftear"][0].GetDouble() == 14.0);
    EXPECT_TRUE(d["faces"][0]["leftear"][1].GetDouble() == 15.0);
    EXPECT_TRUE(d["faces"][0].HasMember("rightear"));
    EXPECT_TRUE(d["faces"][0]["rightear"].IsArray());
    EXPECT_TRUE(d["faces"][0]["rightear"].Size() == 2);
    EXPECT_TRUE(d["faces"][0]["rightear"][0].GetDouble() == 16.0);
    EXPECT_TRUE(d["faces"][0]["rightear"][1].GetDouble() == 17.0);
}

TEST_F(FaceDescriptorTest, jsonfacedesc03_addface_two_test)
{
    FaceDescriptor jfd;

    // 17 elements
    jfd.addFace(1.0f, 2.0f, 3.0f, 4.0f, 5.0f,
                6.0f, 7.0f, 8.0f, 9.0f, 10.0f,
                11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f);

    jfd.addFace(18.0f, 19.0f, 20.0f, 21.0f, 22.0f,
                23.0f, 24.0f, 25.0f, 26.0f, 27.0f,
                28.0f, 29.0f, 30.0f, 31.0f, 32.0f, 33.0f, 34.0f);

    auto json = jfd.toStr();
    std::cout << json << std::endl;

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("faces"));
    EXPECT_TRUE(d["faces"].IsArray());
    EXPECT_TRUE(d["faces"].Size() == 2);
}

TEST_F(FaceDescriptorTest, jsonfacedesc04_add_response_and_returncode)
{
    FaceDescriptor jfd;

    jfd.addResponseName("face_detect");
    jfd.addReturnCode(kAifOk);

    // 17 elements
    jfd.addFace(1.0f, 2.0f, 3.0f, 4.0f, 5.0f,
                6.0f, 7.0f, 8.0f, 9.0f, 10.0f,
                11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f);

    auto json = jfd.toStr();
    std::cout << json << std::endl;

    rj::Document d;
    d.Parse(json.c_str());

    EXPECT_TRUE(d.IsObject());
    EXPECT_TRUE(d.HasMember("response"));
    EXPECT_TRUE(d.HasMember("returnCode"));
    EXPECT_TRUE(d.HasMember("faces"));
    EXPECT_TRUE(d["faces"].IsArray());
    EXPECT_TRUE(d["faces"].Size() == 1);
}