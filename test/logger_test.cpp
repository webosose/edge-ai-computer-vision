/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/log/LogMessage.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using aif::Logger;

class LoggerTest : public ::testing::Test
{
protected:
    LoggerTest() = default;
    ~LoggerTest() = default;

    void SetUp() override
    {
       Logger::init();
    }

    void TearDown() override
    {
    }
};

TEST_F(LoggerTest, 01_Logger)
{
    AIF_FATAL << "AIF_FATAL";
    AIF_ERROR << "AIF_ERROR";
    AIF_WARNING << "AIF_WARNING";
    AIF_INFO << "AIF_INFO";
    AIF_DEBUG << "AIF_DEBUG";
    AIF_VERBOSE << "AIF_VERBOSE";
    AIF_TRACE1 << "AIF_TRACE1";
    AIF_TRACE2 << "AIF_TRACE2";
    AIF_TRACE3 << "AIF_TRACE3";
    AIF_TRACE4 << "AIF_TRACE4";

    Logf("Logf test");
    Loge("Loge test");
    Logw("Logw test");
    Logi("Logi test");
    Logd("Logd test");
    Logv("Logv test");
    TRACE("TRACE test");
    TRACE2("TRACE2 test");
    TRACE3("TRACE3 test");
    TRACE4("TRACE4 test");


    //PmLogLib Restrictions
    // 1. The maximum length of a log message in PmLog function is 1024 bytes.
    //    So, if the message is longer than 1024 bytes, multiple PmLog functions
    //    should be used.
    // 2. PmLog function get msg-id as 2 arguments, and its maximum length is 32 bytes.
    //    So, if the msg-id is longer than 32 bytes, it should be truncated.

    // Test for long string message (1023 + 10)
    std::string str(1023, 'a');
    str += std::string(10, 'b');
    Logi(str.c_str());
    Logw(str.c_str());
    Loge(str.c_str());
    Logf(str.c_str());
    Logd(str.c_str());
    Logv(str.c_str());
    TRACE(str.c_str());
    TRACE2(str.c_str());
    TRACE3(str.c_str());
    TRACE4(str.c_str());

    // Test for long msg-id (31+10)
    // If msg-id is longer than 32 bytes, the log message will not be printed.
    // Instead, the below message will be printed.
    // 'MSGID's length is restricted within 32 characters' by pmloglib.
    std::string msgId(31, 'a');
    msgId += std::string(10, 'b');
    aif::Logger::getInstance().printLog<aif::LogLevel::INFO>(LOG_APPLICATION, LOG_CONTEXT, \
        __func__,  msgId.c_str(), __LINE__, "(1) logging with long msg-id");
    // If msg-id is truncated to 31 bytes, the log message will be printed.
    aif::Logger::getInstance().printLog<aif::LogLevel::INFO>(LOG_APPLICATION, LOG_CONTEXT, \
        __func__,  truncateLength(msgId.c_str(), MAX_FILE_NAME_LENGTH).c_str(), __LINE__, "(2) logging with long msg-id");

    EXPECT_TRUE(true);
}

