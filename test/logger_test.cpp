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

TEST_F(LoggerTest, Logger_test)
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


    EXPECT_TRUE(true);
}

