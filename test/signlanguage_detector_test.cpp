/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/base/DetectorFactory.h>
#include <aif/base/Detector.h>
#include <aif/base/Descriptor.h>
#include <aif/signLanguageArea/SignLanguageAreaDescriptor.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>

using namespace aif;

class SignLanguageAreaDetectorTest : public ::testing::Test
{
protected:
    SignLanguageAreaDetectorTest() = default;
    ~SignLanguageAreaDetectorTest() = default;

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


    void SetUp() override
    {
        DetectorFactory::get().clear();
    }

    void TearDown() override
    {
    }

public:
    static std::string imagePath;
};
std::string SignLanguageAreaDetectorTest::imagePath = "";

TEST_F(SignLanguageAreaDetectorTest, 01_init_signlanguage_model_cpu)
{
    auto fd = DetectorFactory::get().getDetector("signlanguagearea_model_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    EXPECT_EQ(fd->getModelName(), "signlanguagearea_mb2_ssd_lite_float32.tflite");
    auto modelInfo = fd->getModelInfo();
    EXPECT_EQ(modelInfo.height, 300);
    EXPECT_EQ(modelInfo.width, 300);
    EXPECT_EQ(modelInfo.channels, 3);
}

TEST_F(SignLanguageAreaDetectorTest, 02_detect_signlanguagearea)
{
    auto fd = DetectorFactory::get().getDetector("signlanguagearea_model_cpu");
    EXPECT_TRUE(fd.get() != nullptr);
    std::shared_ptr<Descriptor> descriptor = std::make_shared<SignLanguageAreaDescriptor>();
    auto foundSignLanguageArea = std::dynamic_pointer_cast<SignLanguageAreaDescriptor>(descriptor);
    EXPECT_TRUE(fd->detectFromImage(SignLanguageAreaDetectorTest::imagePath, descriptor) == aif::kAifOk);
    Logi("Output: ", foundSignLanguageArea->toStr());
}
