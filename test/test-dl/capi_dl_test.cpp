/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <dlfcn.h>
#include <stdio.h>
#include <string>
#include <opencv2/opencv.hpp>

#include <gtest/gtest.h>
#include <rapidjson/document.h>

namespace rj = rapidjson;

enum DetectorType {
    FACE = 0,
    POSE = 1,
    SEGMENTATION = 2,
};
void *handle = NULL;
bool (*edgeai_isStarted)() = NULL;
bool (*edgeai_startup)(const std::string& basePth) = NULL;
bool (*edgeai_shutdown)() = NULL;
bool (*edgeai_createDetector)(int type, const std::string& option) = NULL;
bool (*edgeai_deleteDetector)(int type) = NULL;
bool (*edgeai_detect)(int type, const cv::Mat &input, std::string &output) = NULL;
bool (*edgeai_detectFromFile)(int type, const std::string &inputPath, std::string &output) = NULL;
bool (*edgeai_detectFromBase64)(int type, const std::string &input, std::string &output) = NULL;

class CApiFacadeDLTest : public ::testing::Test
{
protected:
    CApiFacadeDLTest() = default;
    ~CApiFacadeDLTest() = default;
    void SetUp() override
    {
        basePath = "/usr/share/aif";
        init();
    }

    void TearDown() override
    {
        deinit();
    }

    std::string basePath;
public:
    bool init()
    {
        handle = dlopen("/usr/lib/libedgeai-vision.so.1", RTLD_LAZY);
        if(handle == NULL) {
            printf("error %s\n", dlerror());
            return false;
        }

        const char* funcIsStarted = "edgeai_isStarted";
        edgeai_isStarted = (bool (*)()) dlsym(handle, funcIsStarted);
        if(dlerror() != NULL) {
            printf("dlsym error: %s (%s)\n", funcIsStarted, dlerror());
            dlclose(handle);
            handle = NULL;
            return false;
        }

        const char* funcStartup = "edgeai_startup";
        edgeai_startup = (bool (*)(const std::string& basePath)) dlsym(handle, funcStartup);
        if(dlerror() != NULL) {
            printf("dlsym error: %s (%s)\n", funcStartup, dlerror());
            dlclose(handle);
            handle = NULL;
            return false;
        }

        const char* funcShutdown = "edgeai_shutdown";
        edgeai_shutdown= (bool (*)()) dlsym(handle, funcShutdown);
        if(dlerror() != NULL) {
            printf("dlsym error: %s (%s)\n", funcShutdown, dlerror());
            dlclose(handle);
            handle = NULL;
            return false;
        }

        const char* funcCreateDetector = "edgeai_createDetector";
        edgeai_createDetector = (bool (*)(int, const std::string&)) dlsym(handle, funcCreateDetector);
        if(dlerror() != NULL) {
            printf("dlsym error: %s (%s)\n", funcCreateDetector, dlerror());
            dlclose(handle);
            handle = NULL;
            return false;
        }

        const char* funcDeleteDetector = "edgeai_deleteDetector";
        edgeai_deleteDetector = (bool (*)(int)) dlsym(handle, funcDeleteDetector);
        if(dlerror() != NULL) {
            printf("dlsym error: %s (%s)\n", funcDeleteDetector, dlerror());
            dlclose(handle);
            handle = NULL;
            return false;
        }

        const char* funcDetect = "edgeai_detect";
        edgeai_detect = (bool (*)(int, const cv::Mat&, std::string&)) dlsym(handle, funcDetect);
        if(dlerror() != NULL) {
            printf("dlsym error: %s (%s)\n", funcDetect, dlerror());
            dlclose(handle);
            handle = NULL;
            return false;
        }

        const char* funcDetectFromFile = "edgeai_detectFromFile";
        edgeai_detectFromFile = (bool (*)(int, const std::string&, std::string&)) dlsym(handle, funcDetectFromFile);
        if(dlerror() != NULL) {
            printf("dlsym error: %s (%s)\n", funcDetect, dlerror());
            dlclose(handle);
            handle = NULL;
            return false;
        }

        const char* funcDetectFromBase64 = "edgeai_detectFromBase64";
        edgeai_detectFromBase64= (bool (*)(int, const std::string&, std::string&)) dlsym(handle, funcDetectFromBase64);
        if(dlerror() != NULL) {
            printf("dlsym error: %s (%s)\n", funcDetectFromBase64, dlerror());
            dlclose(handle);
            handle = NULL;
            return false;
        }


        return true;
    }

    bool deinit() {
        if (handle) {
            dlclose(handle);
            handle = NULL;
        }
        return true;
    }

};

TEST_F(CApiFacadeDLTest, 01_startup)
{
    EXPECT_FALSE(edgeai_isStarted());
    EXPECT_TRUE(edgeai_startup(""));
    EXPECT_TRUE(edgeai_isStarted());
    EXPECT_TRUE(edgeai_shutdown());
    EXPECT_FALSE(edgeai_isStarted());

    EXPECT_TRUE(edgeai_startup("/test"));
    EXPECT_TRUE(edgeai_isStarted());
    EXPECT_TRUE(edgeai_shutdown());
    EXPECT_FALSE(edgeai_isStarted());
}

TEST_F(CApiFacadeDLTest, 02_createDetector_default)
{
    EXPECT_TRUE(edgeai_startup(""));
    EXPECT_TRUE(edgeai_isStarted());
    EXPECT_TRUE(edgeai_createDetector(DetectorType::FACE, ""));
    EXPECT_TRUE(edgeai_createDetector(DetectorType::POSE, ""));
    EXPECT_TRUE(edgeai_createDetector(DetectorType::SEGMENTATION, ""));
    EXPECT_TRUE(edgeai_shutdown());
}

TEST_F(CApiFacadeDLTest, 03_deleteDetector_default)
{
    EXPECT_TRUE(edgeai_startup(""));
    EXPECT_TRUE(edgeai_isStarted());
    EXPECT_FALSE(edgeai_deleteDetector(DetectorType::FACE));
    EXPECT_FALSE(edgeai_deleteDetector(DetectorType::POSE));
    EXPECT_FALSE(edgeai_deleteDetector(DetectorType::SEGMENTATION));

    EXPECT_TRUE(edgeai_createDetector(DetectorType::FACE, ""));
    EXPECT_TRUE(edgeai_createDetector(DetectorType::POSE, ""));
    EXPECT_TRUE(edgeai_createDetector(DetectorType::SEGMENTATION, ""));

    EXPECT_TRUE(edgeai_deleteDetector(DetectorType::FACE));
    EXPECT_TRUE(edgeai_deleteDetector(DetectorType::POSE));
    EXPECT_TRUE(edgeai_deleteDetector(DetectorType::SEGMENTATION));
    EXPECT_TRUE(edgeai_shutdown());
}

TEST_F(CApiFacadeDLTest, 04_detect_face_default)
{
    DetectorType type = DetectorType::FACE;

    EXPECT_TRUE(edgeai_startup(""));
    EXPECT_TRUE(edgeai_isStarted());

    cv::Mat input = cv::imread(basePath + "/images/person.jpg", cv::IMREAD_COLOR);
    std::string output;

    EXPECT_TRUE(edgeai_createDetector(type, ""));
    EXPECT_TRUE(edgeai_detect(type, input, output));
    EXPECT_TRUE(edgeai_deleteDetector(type));
    EXPECT_TRUE(edgeai_shutdown());

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["faces"].Size(), 1);
    std::cout << output << std::endl;
}

TEST_F(CApiFacadeDLTest, 05_detectFromFile_face_default)
{
    DetectorType type = DetectorType::FACE;

    EXPECT_TRUE(edgeai_startup(""));
    EXPECT_TRUE(edgeai_isStarted());

    std::string inputPath = basePath + "/images/person.jpg";
    std::string output;
    EXPECT_TRUE(edgeai_createDetector(type, ""));
    EXPECT_TRUE(edgeai_detectFromFile(type, inputPath, output));
    EXPECT_TRUE(edgeai_deleteDetector(type));
    EXPECT_TRUE(edgeai_shutdown());

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["faces"].Size(), 1);
    std::cout << output << std::endl;
}

TEST_F(CApiFacadeDLTest, 06_detect_pose_default)
{
    DetectorType type = DetectorType::POSE;

    EXPECT_TRUE(edgeai_startup(""));
    EXPECT_TRUE(edgeai_isStarted());

    cv::Mat input = cv::imread(basePath + "/images/person.jpg", cv::IMREAD_COLOR);
    std::string output;

    EXPECT_TRUE(edgeai_createDetector(type, ""));
    EXPECT_TRUE(edgeai_detect(type, input, output));
    EXPECT_TRUE(edgeai_deleteDetector(type));
    EXPECT_TRUE(edgeai_shutdown());

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["poses"].Size(), 1);
    std::cout << output << std::endl;
}

TEST_F(CApiFacadeDLTest, 07_detectFromFile_pose_default)
{
    DetectorType type = DetectorType::POSE;

    EXPECT_TRUE(edgeai_startup(""));
    EXPECT_TRUE(edgeai_isStarted());

    std::string inputPath = basePath + "/images/person.jpg";
    std::string output;
    EXPECT_TRUE(edgeai_createDetector(type, ""));
    EXPECT_TRUE(edgeai_detectFromFile(type, inputPath, output));
    EXPECT_TRUE(edgeai_deleteDetector(type));
    EXPECT_TRUE(edgeai_shutdown());

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["poses"].Size(), 1);
    std::cout << output << std::endl;
}

TEST_F(CApiFacadeDLTest, 08_detect_segmentation_default)
{
    DetectorType type = DetectorType::SEGMENTATION;

    EXPECT_TRUE(edgeai_startup(""));
    EXPECT_TRUE(edgeai_isStarted());

    cv::Mat input = cv::imread(basePath + "/images/person.jpg", cv::IMREAD_COLOR);
    std::string output;

    EXPECT_TRUE(edgeai_createDetector(type, ""));
    EXPECT_TRUE(edgeai_detect(type, input, output));
    EXPECT_TRUE(edgeai_deleteDetector(type));
    EXPECT_TRUE(edgeai_shutdown());

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["segments"].Size(), 1);
}

TEST_F(CApiFacadeDLTest, 09_detectFromFile_segmentation_default)
{
    DetectorType type = DetectorType::SEGMENTATION;
    EXPECT_TRUE(edgeai_startup(""));
    EXPECT_TRUE(edgeai_isStarted());

    std::string inputPath = basePath + "/images/person.jpg";
    std::string output;
    EXPECT_TRUE(edgeai_createDetector(type, ""));
    EXPECT_TRUE(edgeai_detectFromFile(type, inputPath, output));
    EXPECT_TRUE(edgeai_deleteDetector(type));
    EXPECT_TRUE(edgeai_shutdown());

    rj::Document result;
    result.Parse(output.c_str());
    EXPECT_EQ(result["segments"].Size(), 1);
}

TEST_F(CApiFacadeDLTest, 10_create_delete_detector)
{
    EXPECT_TRUE(edgeai_startup(""));
    EXPECT_TRUE(edgeai_isStarted());
    EXPECT_TRUE(edgeai_createDetector(DetectorType::FACE, ""));
    EXPECT_FALSE(edgeai_createDetector(DetectorType::FACE, ""));
    EXPECT_TRUE(edgeai_deleteDetector(DetectorType::FACE));
    EXPECT_FALSE(edgeai_deleteDetector(DetectorType::FACE));
    EXPECT_TRUE(edgeai_createDetector(DetectorType::FACE, ""));
    EXPECT_TRUE(edgeai_shutdown());
}

TEST_F(CApiFacadeDLTest, 11_use_api_before_startup)
{
    cv::Mat input = cv::imread(basePath + "/images/person.jpg", cv::IMREAD_COLOR);
    std::string output;

    EXPECT_FALSE(edgeai_isStarted());
    EXPECT_FALSE(edgeai_createDetector(DetectorType::FACE, ""));
    EXPECT_FALSE(edgeai_deleteDetector(DetectorType::FACE));
    EXPECT_FALSE(edgeai_detect(DetectorType::FACE, input, output));
    EXPECT_FALSE(edgeai_shutdown());
}
