/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/facade/EdgeAIVision.h>
#include <aif/base/AIVision.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <gtest/gtest.h>

using namespace aif;
namespace rj = rapidjson;

class ApiFacadeTest : public ::testing::Test
{
protected:
    ApiFacadeTest() = default;
    ~ApiFacadeTest() = default;
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};



TEST_F(ApiFacadeTest, 01_startup)
{
    EdgeAIVision ai = EdgeAIVision::getInstance();
    EXPECT_FALSE(ai.isStarted());

    ai.startup();
    EXPECT_TRUE(ai.isStarted());
    ai.shutdown();
    EXPECT_FALSE(ai.isStarted());

    ai.startup("/test");
    EXPECT_TRUE(ai.isStarted());
    ai.shutdown();
    EXPECT_FALSE(ai.isStarted());
}

TEST_F(ApiFacadeTest, 02_createDetector_default)
{
    EdgeAIVision ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());
    EXPECT_TRUE(ai.createDetector(EdgeAIVision::DetectorType::FACE));
    EXPECT_TRUE(ai.createDetector(EdgeAIVision::DetectorType::POSE));
    EXPECT_TRUE(ai.createDetector(EdgeAIVision::DetectorType::SEGMENTATION));
    ai.shutdown();
}

TEST_F(ApiFacadeTest, 03_deleteDetector_default)
{
    EdgeAIVision ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());
    EXPECT_FALSE(ai.deleteDetector(EdgeAIVision::DetectorType::FACE));
    EXPECT_FALSE(ai.deleteDetector(EdgeAIVision::DetectorType::POSE));
    EXPECT_FALSE(ai.deleteDetector(EdgeAIVision::DetectorType::SEGMENTATION));

    EXPECT_TRUE(ai.createDetector(EdgeAIVision::DetectorType::FACE));
    EXPECT_TRUE(ai.createDetector(EdgeAIVision::DetectorType::POSE));
    EXPECT_TRUE(ai.createDetector(EdgeAIVision::DetectorType::SEGMENTATION));

    EXPECT_TRUE(ai.deleteDetector(EdgeAIVision::DetectorType::FACE));
    EXPECT_TRUE(ai.deleteDetector(EdgeAIVision::DetectorType::POSE));
    EXPECT_TRUE(ai.deleteDetector(EdgeAIVision::DetectorType::SEGMENTATION));
    ai.shutdown();
}

TEST_F(ApiFacadeTest, 04_detect_face_default)
{
    EdgeAIVision::DetectorType type = EdgeAIVision::DetectorType::FACE;
    EdgeAIVision ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());

    std::string basePath = AIVision::getBasePath();
    cv::Mat input = cv::imread(basePath + "/images/mona.jpg", cv::IMREAD_COLOR);
    std::string output;

    EXPECT_TRUE(ai.createDetector(type));
    EXPECT_TRUE(ai.detect(type, input, output));
    EXPECT_TRUE(ai.deleteDetector(type));
    ai.shutdown();

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["faces"].Size(), 1);
    Logd(output);
}

TEST_F(ApiFacadeTest, 05_detectFromFile_face_default)
{
    EdgeAIVision::DetectorType type = EdgeAIVision::DetectorType::FACE;
    EdgeAIVision ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());

    std::string inputPath = AIVision::getBasePath() + "/images/mona.jpg";
    std::string output;
    EXPECT_TRUE(ai.createDetector(type));
    EXPECT_TRUE(ai.detectFromFile(type, inputPath, output));
    EXPECT_TRUE(ai.deleteDetector(type));
    ai.shutdown();

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["faces"].Size(), 1);
    Logd(output);
}

TEST_F(ApiFacadeTest, 06_detectFromBase64_face_default)
{
    EdgeAIVision::DetectorType type = EdgeAIVision::DetectorType::FACE;
    EdgeAIVision ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());

    auto base64image = aif::fileToStr(AIVision::getBasePath() + "/images/mona_base64.txt"); // 128 x 128
    std::string output;
    EXPECT_TRUE(ai.createDetector(type));
    EXPECT_TRUE(ai.detectFromBase64(type, base64image, output));
    EXPECT_TRUE(ai.deleteDetector(type));
    ai.shutdown();

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["faces"].Size(), 1);
    Logd(output);
}

TEST_F(ApiFacadeTest, 07_detect_pose_default)
{
    EdgeAIVision::DetectorType type = EdgeAIVision::DetectorType::POSE;
    EdgeAIVision ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());

    std::string basePath = AIVision::getBasePath();
    cv::Mat input = cv::imread(basePath + "/images/person.jpg", cv::IMREAD_COLOR);
    std::string output;

    EXPECT_TRUE(ai.createDetector(type));
    EXPECT_TRUE(ai.detect(type, input, output));
    EXPECT_TRUE(ai.deleteDetector(type));
    ai.shutdown();

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["poses"].Size(), 1);
    Logd(output);
}

TEST_F(ApiFacadeTest, 08_detectFromFile_pose_default)
{
    EdgeAIVision::DetectorType type = EdgeAIVision::DetectorType::POSE;
    EdgeAIVision ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());

    std::string inputPath = AIVision::getBasePath() + "/images/person.jpg";
    std::string output;
    EXPECT_TRUE(ai.createDetector(type));
    EXPECT_TRUE(ai.detectFromFile(type, inputPath, output));
    EXPECT_TRUE(ai.deleteDetector(type));
    ai.shutdown();

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["poses"].Size(), 1);
    Logd(output);
}

TEST_F(ApiFacadeTest, 09_detect_segmentation_default)
{
    EdgeAIVision::DetectorType type = EdgeAIVision::DetectorType::SEGMENTATION;
    EdgeAIVision ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());

    std::string basePath = AIVision::getBasePath();
    cv::Mat input = cv::imread(basePath + "/images/person.jpg", cv::IMREAD_COLOR);
    std::string output;

    EXPECT_TRUE(ai.createDetector(type));
    EXPECT_TRUE(ai.detect(type, input, output));
    EXPECT_TRUE(ai.deleteDetector(type));
    ai.shutdown();

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["segments"].Size(), 1);
    //Logd(output);
}

TEST_F(ApiFacadeTest, 10_detectFromFile_segmentation_default)
{
    EdgeAIVision::DetectorType type = EdgeAIVision::DetectorType::SEGMENTATION;
    EdgeAIVision ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());

    std::string inputPath = AIVision::getBasePath() + "/images/person.jpg";
    std::string output;
    EXPECT_TRUE(ai.createDetector(type));
    EXPECT_TRUE(ai.detectFromFile(type, inputPath, output));
    EXPECT_TRUE(ai.deleteDetector(type));
    ai.shutdown();

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["segments"].Size(), 1);
}

TEST_F(ApiFacadeTest, 11_detect_face_full_range)
{
    std::string param =
        "{ \
            \"model\" : \"face_full_range_cpu\",    \
            \"param\": { \
                \"common\" : { \
                    \"useXnnpack\": true, \
                    \"numThreads\": 1 \
                },\
                \"modelParam\": { \
                    \"strides\": [4], \
                    \"optAspectRatios\": [1.0], \
                    \"interpolatedScaleAspectRatio\": 0.0, \
                    \"anchorOffsetX\": 0.5, \
                    \"anchorOffsetY\": 0.5, \
                    \"minScale\": 0.1484375, \
                    \"maxScale\": 0.75, \
                    \"reduceBoxesInLowestLayer\": false, \
                    \"scoreThreshold\": 0.5, \
                    \"iouThreshold\": 0.2, \
                    \"maxOutputSize\": 100, \
                    \"updateThreshold\": 0.3 \
                } \
             } \
        }";

    EdgeAIVision::DetectorType type = EdgeAIVision::DetectorType::FACE;
    EdgeAIVision ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());

    std::string basePath = AIVision::getBasePath();
    cv::Mat input = cv::imread(basePath + "/images/mona.jpg", cv::IMREAD_COLOR);
    std::string output;

    EXPECT_TRUE(ai.createDetector(type, param));
    EXPECT_TRUE(ai.detect(type, input, output));
    EXPECT_TRUE(ai.deleteDetector(type));
    ai.shutdown();

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["faces"].Size(), 1);
    Logd(output);
}

TEST_F(ApiFacadeTest, 12_create_delete_detector)
{
    EdgeAIVision ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());
    EXPECT_TRUE(ai.createDetector(EdgeAIVision::DetectorType::FACE));
    EXPECT_FALSE(ai.createDetector(EdgeAIVision::DetectorType::FACE));
    EXPECT_TRUE(ai.deleteDetector(EdgeAIVision::DetectorType::FACE));
    EXPECT_FALSE(ai.deleteDetector(EdgeAIVision::DetectorType::FACE));
    EXPECT_TRUE(ai.createDetector(EdgeAIVision::DetectorType::FACE));
    ai.shutdown();
}

TEST_F(ApiFacadeTest, 13_use_api_before_startup)
{
    std::string basePath = AIVision::getBasePath();
    cv::Mat input = cv::imread(basePath + "/images/mona.jpg", cv::IMREAD_COLOR);
    std::string output;

    EdgeAIVision ai = EdgeAIVision::getInstance();
    EXPECT_FALSE(ai.isStarted());
    EXPECT_FALSE(ai.createDetector(EdgeAIVision::DetectorType::FACE));
    EXPECT_FALSE(ai.deleteDetector(EdgeAIVision::DetectorType::FACE));
    EXPECT_FALSE(ai.detect(EdgeAIVision::DetectorType::FACE, input, output));
    ai.shutdown();
}
