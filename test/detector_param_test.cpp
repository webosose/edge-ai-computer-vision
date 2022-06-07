/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/DetectorParam.h>

#include <gtest/gtest.h>
#include <iostream>

using namespace aif;

class DetectorParamTest : public ::testing::Test {
  protected:
    DetectorParamTest() {
        param_xnnpack_on = "{"
                           "  \"common\" : {"
                           "      \"useXnnpack\": true,"
                           "      \"numThreads\": 1"
                           "    }"
                           "}";
        param_xnnpack_off = "{"
                            "  \"common\" : {"
                            "      \"useXnnpack\": false,"
                            "      \"numThreads\": 1"
                            "    }"
                            "}";
        param_numThreads_4 = "{"
                             "  \"common\" : {"
                             "      \"useXnnpack\": true,"
                             "      \"numThreads\": 4"
                             "    }"
                             "}";
        param_delegates_1 = "{"
                            "  \"common\" : {"
                            "      \"useXnnpack\": true,"
                            "      \"numThreads\": 1"
                            "    },"
                            "  \"delegates\" : ["
                            "    {"
                            "      \"name\": \"armnn_delegate\","
                            "      \"option\": {"
                            "        \"backends\": [\"CpuAcc\"],"
                            "        \"logging-severity\": \"info\""
                            "      }"
                            "    }"
                            "  ]"
                            "}";
        param_delegates_2 = "{"
                            "  \"common\" : {"
                            "      \"useXnnpack\": true,"
                            "      \"numThreads\": 1"
                            "    },"
                            "  \"delegates\" : ["
                            "    {"
                            "      \"name\": \"armnn_delegate\","
                            "      \"option\": {"
                            "        \"backends\": [\"CpuAcc\"],"
                            "        \"logging-severity\": \"info\""
                            "      }"
                            "    },"
                            "    {"
                            "      \"name\": \"test_delegate\","
                            "      \"option\": {}"
                            "    }"
                            "  ]"
                            "}";
    }
    ~DetectorParamTest() = default;

    void SetUp() override {}

    void TearDown() override {}

    std::string param_xnnpack_on;
    std::string param_xnnpack_off;
    std::string param_numThreads_4;
    std::string param_delegates_1;
    std::string param_delegates_2;
};

TEST_F(DetectorParamTest, 01_fromJson) {
    DetectorParam param;
    EXPECT_EQ(param.fromJson(param_xnnpack_on), kAifOk);
}

TEST_F(DetectorParamTest, 02_getUseXnnpack) {
    DetectorParam param;
    EXPECT_TRUE(param.getUseXnnpack());
    EXPECT_EQ(param.fromJson(param_xnnpack_off), kAifOk);
    EXPECT_FALSE(param.getUseXnnpack());

    DetectorParam param2;
    EXPECT_EQ(param2.fromJson(param_xnnpack_on), kAifOk);
    EXPECT_TRUE(param2.getUseXnnpack());
}

TEST_F(DetectorParamTest, 03_getNumThreads) {
    DetectorParam param;
    EXPECT_EQ(param.getNumThreads(), 1);
    EXPECT_EQ(param.fromJson(param_numThreads_4), kAifOk);
    EXPECT_EQ(param.getNumThreads(), 4);
}

TEST_F(DetectorParamTest, 04_getDelegates) {
    DetectorParam param;
    EXPECT_EQ(param.getDelegates().size(), 0);

    EXPECT_EQ(param.fromJson(param_delegates_1), kAifOk);
    EXPECT_EQ(param.getDelegates().size(), 1);

    DetectorParam param2;
    EXPECT_EQ(param2.fromJson(param_delegates_2), kAifOk);
    EXPECT_EQ(param2.getDelegates().size(), 2);
}
