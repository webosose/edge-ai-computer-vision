/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/facade/EdgeAIVision.h>
#include <aif/base/AIVision.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <gtest/gtest.h>
#include <sstream>
#include <boost/filesystem.hpp>

using namespace aif;
namespace rj = rapidjson;

class SignLanguageAreaApiTest : public ::testing::Test
{
protected:
    SignLanguageAreaApiTest() = default;
    ~SignLanguageAreaApiTest() = default;
    static void SetUpTestCase()
    {
        AIVision::init();
        std::string imageSubPath = "/images/signlanguagearea.png";
        std::vector<std::string> imagePaths = {
            AIVision::getBasePath() + imageSubPath
        };
        for(auto& _path : imagePaths) {
            if (boost::filesystem::exists(_path)) {
                imagePath = _path;
                break;
            }
        }
        if (imagePath.empty()) {
            std::ostringstream ssearchedPath;
            std::copy(imagePaths.begin(), imagePaths.end() - 1, std::ostream_iterator<std::string>(ssearchedPath, " or "));
            ssearchedPath << imagePaths.back();
            FAIL() << "Image file not found at " << ssearchedPath.str();
        }
    }

    static void TearDownTestCase()
    {
        AIVision::deinit();
    }

public:
    static std::string imagePath;
};
std::string SignLanguageAreaApiTest::imagePath = "";

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

    cv::Mat input;
    std::string output;
    input = cv::imread(SignLanguageAreaApiTest::imagePath, cv::IMREAD_COLOR);

    EXPECT_TRUE(ai.createDetector(type));
    EXPECT_TRUE(ai.detect(type, input, output));
    EXPECT_TRUE(ai.deleteDetector(type));
    ai.shutdown();

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["signLanguageAreas"].Size(), 5);
    Logd(output);
}
