/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/facade/EdgeAIVisionC.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <gtest/gtest.h>

using namespace aif;
namespace rj = rapidjson;

class CApiFacadeTest : public ::testing::Test
{
protected:
    CApiFacadeTest() = default;
    ~CApiFacadeTest() = default;
    void SetUp() override
    {
        basePath = "/usr/share/aif";
    }

    void TearDown() override
    {
    }

    std::string basePath;
};



TEST_F(CApiFacadeTest, 01_startup)
{
    EXPECT_FALSE(edgeai_isStarted());

    edgeai_startup("");
    EXPECT_TRUE(edgeai_isStarted());
    edgeai_shutdown();
    EXPECT_FALSE(edgeai_isStarted());

    edgeai_startup("/test");
    EXPECT_TRUE(edgeai_isStarted());
    edgeai_shutdown();
    EXPECT_FALSE(edgeai_isStarted());
}

TEST_F(CApiFacadeTest, 02_createDetector_default)
{
    edgeai_startup("");
    EXPECT_TRUE(edgeai_isStarted());
    EXPECT_TRUE(edgeai_createDetector(DetectorType::FACE, ""));
#ifndef USE_UPDATABLE_MODELS
    EXPECT_TRUE(edgeai_createDetector(DetectorType::POSE, ""));
    EXPECT_TRUE(edgeai_createDetector(DetectorType::SEGMENTATION, ""));
#endif
    edgeai_shutdown();
}

TEST_F(CApiFacadeTest, 03_deleteDetector_default)
{
    edgeai_startup();
    EXPECT_TRUE(edgeai_isStarted());
    EXPECT_FALSE(edgeai_deleteDetector(DetectorType::FACE));
#ifndef USE_UPDATABLE_MODELS
    EXPECT_FALSE(edgeai_deleteDetector(DetectorType::POSE));
    EXPECT_FALSE(edgeai_deleteDetector(DetectorType::SEGMENTATION));
#endif

    EXPECT_TRUE(edgeai_createDetector(DetectorType::FACE, ""));
#ifndef USE_UPDATABLE_MODELS
    EXPECT_TRUE(edgeai_createDetector(DetectorType::POSE, ""));
    EXPECT_TRUE(edgeai_createDetector(DetectorType::SEGMENTATION, ""));
#endif

    EXPECT_TRUE(edgeai_deleteDetector(DetectorType::FACE));
#ifndef USE_UPDATABLE_MODELS
    EXPECT_TRUE(edgeai_deleteDetector(DetectorType::POSE));
    EXPECT_TRUE(edgeai_deleteDetector(DetectorType::SEGMENTATION));
#endif
    edgeai_shutdown();
}

TEST_F(CApiFacadeTest, 04_detect_face_default)
{
    DetectorType type = DetectorType::FACE;
    edgeai_startup();
    ASSERT_TRUE(edgeai_isStarted());

    cv::Mat input = cv::imread(basePath + "/images/person.jpg", cv::IMREAD_COLOR);
    std::string output;

    ASSERT_TRUE(edgeai_createDetector(type, ""));
    ASSERT_TRUE(edgeai_detect(type, input, output));
    EXPECT_TRUE(edgeai_deleteDetector(type));
    edgeai_shutdown();

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["faces"].Size(), 1);
    Logd(output);
}

TEST_F(CApiFacadeTest, 05_detectFromFile_face_default)
{
    DetectorType type = DetectorType::FACE;
    edgeai_startup();
    ASSERT_TRUE(edgeai_isStarted());

    std::string inputPath = basePath + "/images/person.jpg";
    std::string output;
    ASSERT_TRUE(edgeai_createDetector(type, ""));
    ASSERT_TRUE(edgeai_detectFromFile(type, inputPath, output));
    EXPECT_TRUE(edgeai_deleteDetector(type));
    edgeai_shutdown();

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["faces"].Size(), 1);
    Logd(output);
}

#ifndef USE_UPDATABLE_MODELS
TEST_F(CApiFacadeTest, 06_detect_pose_default)
{
    DetectorType type = DetectorType::POSE;
    edgeai_startup();
    ASSERT_TRUE(edgeai_isStarted());

    cv::Mat input = cv::imread(basePath + "/images/person.jpg", cv::IMREAD_COLOR);
    std::string output;

    ASSERT_TRUE(edgeai_createDetector(type, ""));
    ASSERT_TRUE(edgeai_detect(type, input, output));
    EXPECT_TRUE(edgeai_deleteDetector(type));
    edgeai_shutdown();

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["poses"].Size(), 1);
    Logd(output);
}

TEST_F(CApiFacadeTest, 07_detectFromFile_pose_default)
{
    DetectorType type = DetectorType::POSE;
    edgeai_startup();
    ASSERT_TRUE(edgeai_isStarted());

    std::string inputPath = basePath + "/images/person.jpg";
    std::string output;
    ASSERT_TRUE(edgeai_createDetector(type, ""));
    ASSERT_TRUE(edgeai_detectFromFile(type, inputPath, output));
    EXPECT_TRUE(edgeai_deleteDetector(type));
    edgeai_shutdown();

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["poses"].Size(), 1);
    Logd(output);
}

TEST_F(CApiFacadeTest, 08_detect_segmentation_default)
{
    DetectorType type = DetectorType::SEGMENTATION;
    edgeai_startup();
    ASSERT_TRUE(edgeai_isStarted());

    cv::Mat input = cv::imread(basePath + "/images/person.jpg", cv::IMREAD_COLOR);
    std::string output;

    ASSERT_TRUE(edgeai_createDetector(type, ""));
    ASSERT_TRUE(edgeai_detect(type, input, output));
    EXPECT_TRUE(edgeai_deleteDetector(type));
    edgeai_shutdown();

    rj::Document result;
    result.Parse(output.c_str());
    ASSERT_TRUE(result.HasMember("segments"));
    EXPECT_EQ(result["segments"].Size(), 1);
    //Logd(output);
}

TEST_F(CApiFacadeTest, 09_detectFromFile_segmentation_default)
{
    DetectorType type = DetectorType::SEGMENTATION;
    edgeai_startup();
    ASSERT_TRUE(edgeai_isStarted());

    std::string inputPath = basePath + "/images/person.jpg";
    std::string output;
    ASSERT_TRUE(edgeai_createDetector(type, ""));
    ASSERT_TRUE(edgeai_detectFromFile(type, inputPath, output));
    EXPECT_TRUE(edgeai_deleteDetector(type));
    edgeai_shutdown();

    rj::Document result;
    result.Parse(output.c_str());
    ASSERT_TRUE(result.HasMember("segments"));
    EXPECT_EQ(result["segments"].Size(), 1);
}
#endif

TEST_F(CApiFacadeTest, 10_create_delete_detector)
{
    edgeai_startup();
    ASSERT_TRUE(edgeai_isStarted());
    EXPECT_TRUE(edgeai_createDetector(DetectorType::FACE, ""));
    EXPECT_FALSE(edgeai_createDetector(DetectorType::FACE, ""));
    EXPECT_TRUE(edgeai_deleteDetector(DetectorType::FACE));
    EXPECT_FALSE(edgeai_deleteDetector(DetectorType::FACE));
    EXPECT_TRUE(edgeai_createDetector(DetectorType::FACE, ""));
    edgeai_shutdown();
}

TEST_F(CApiFacadeTest, 11_use_api_before_startup)
{
    cv::Mat input = cv::imread(basePath + "/images/person.jpg", cv::IMREAD_COLOR);
    std::string output;

    EXPECT_FALSE(edgeai_isStarted());
    EXPECT_FALSE(edgeai_createDetector(DetectorType::FACE, ""));
    EXPECT_FALSE(edgeai_deleteDetector(DetectorType::FACE));
    EXPECT_FALSE(edgeai_detect(DetectorType::FACE, input, output));
    edgeai_shutdown();
}
