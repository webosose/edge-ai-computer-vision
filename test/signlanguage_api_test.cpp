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

class SignLanguageAreaApiTest : public ::testing::Test
{
protected:
    SignLanguageAreaApiTest() = default;
    ~SignLanguageAreaApiTest() = default;
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(SignLanguageAreaApiTest, createDetector_default)
{
    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());
    EXPECT_TRUE(ai.createDetector(EdgeAIVision::DetectorType::SIGNLANGUAGEAREA));
    ai.shutdown();
}

TEST_F(SignLanguageAreaApiTest, deleteDetector_default)
{
    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());

    EXPECT_FALSE(ai.deleteDetector(EdgeAIVision::DetectorType::SIGNLANGUAGEAREA));
    EXPECT_TRUE(ai.createDetector(EdgeAIVision::DetectorType::SIGNLANGUAGEAREA));
    EXPECT_TRUE(ai.deleteDetector(EdgeAIVision::DetectorType::SIGNLANGUAGEAREA));
    ai.shutdown();
}

TEST_F(SignLanguageAreaApiTest, detect_signlanguagearea)
{
    EdgeAIVision::DetectorType type = EdgeAIVision::DetectorType::SIGNLANGUAGEAREA;
    EdgeAIVision& ai = EdgeAIVision::getInstance();
    ai.startup();
    EXPECT_TRUE(ai.isStarted());

    std::string basePath = AIVision::getBasePath();
    cv::Mat input = cv::imread(basePath + "/images/signlanguagearea.png", cv::IMREAD_COLOR);
    std::string output;

    EXPECT_TRUE(ai.createDetector(type));
    EXPECT_TRUE(ai.detect(type, input, output));
    EXPECT_TRUE(ai.deleteDetector(type));
    ai.shutdown();

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["signLanguageAreas"].Size(), 5);
    Logd(output);
}
