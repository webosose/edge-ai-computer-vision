/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/facade/EdgeAIVision.h>
#include <aif/base/AIVision.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <gtest/gtest.h>

using namespace aif;
namespace rj = rapidjson;

class TextApiTest : public ::testing::Test
{
protected:
    TextApiTest() = default;
    ~TextApiTest() = default;
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(TextApiTest, createDetector_default)
{
    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());
    EXPECT_TRUE(ai.createDetector(EdgeAIVision::DetectorType::TEXT));
    ai.shutdown();
}

TEST_F(TextApiTest, deleteDetector_default)
{
    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());

    EXPECT_FALSE(ai.deleteDetector(EdgeAIVision::DetectorType::TEXT));
    EXPECT_TRUE(ai.createDetector(EdgeAIVision::DetectorType::TEXT));
    EXPECT_TRUE(ai.deleteDetector(EdgeAIVision::DetectorType::TEXT));
    ai.shutdown();
}

TEST_F(TextApiTest, detect_text)
{
    EdgeAIVision::DetectorType type = EdgeAIVision::DetectorType::TEXT;
    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());

    std::string basePath = AIVision::getBasePath();
    cv::Mat input = cv::imread(basePath + "/images/text.jpg", cv::IMREAD_COLOR);
    std::string output;

    EXPECT_TRUE(ai.createDetector(type));
    EXPECT_TRUE(ai.detect(type, input, output));
    EXPECT_TRUE(ai.deleteDetector(type));
    ai.shutdown();

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["texts"]["bbox"].Size(), 19);
    EXPECT_EQ(result["texts"]["box"].Size(), 19);
    Logd(output);
}

TEST_F(TextApiTest, detect_text_with_detection_region)
{
    std::string option = R"(
        {
            "model": "text_paddleocr_320_v2",
            "param": {
                "modelParam" : {
                    "detectionRegion" : [500, 300, 800, 400]
                }
            }
        })";

    EdgeAIVision::DetectorType type = EdgeAIVision::DetectorType::TEXT;
    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());

    std::string basePath = AIVision::getBasePath();
    cv::Mat input = cv::imread(basePath + "/images/text.jpg", cv::IMREAD_COLOR);
    std::string output;

    EXPECT_TRUE(ai.createDetector(type, option));
    EXPECT_TRUE(ai.detect(type, input, output));
    EXPECT_TRUE(ai.deleteDetector(type));
    ai.shutdown();

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["texts"]["bbox"].Size(), 6);
    EXPECT_EQ(result["texts"]["box"].Size(), 6);
    Logd(output);
}

TEST_F(TextApiTest, detectFromFile_text)
{
    EdgeAIVision::DetectorType type = EdgeAIVision::DetectorType::TEXT;
    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());

    std::string inputPath = AIVision::getBasePath() + "/images/text.jpg";
    std::string output;
    EXPECT_TRUE(ai.createDetector(type));
    EXPECT_TRUE(ai.detectFromFile(type, inputPath, output));
    EXPECT_TRUE(ai.deleteDetector(type));
    ai.shutdown();

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["texts"]["bbox"].Size(), 19);
    EXPECT_EQ(result["texts"]["box"].Size(), 19);
    Logd(output);
}
