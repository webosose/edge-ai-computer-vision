#include <aif/tools/Stopwatch.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

using aif::Stopwatch;
using aif::Logger;

class StopwatchTest : public ::testing::Test
{
protected:
    StopwatchTest() = default;
    ~StopwatchTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(StopwatchTest, sw01_lap_timer)
{
    Stopwatch sw;
    sw.start();
    sw.sleep(5);
    auto dt1 = sw.getMs();
    TRACE("dt1: ", dt1);
    EXPECT_TRUE(dt1 >= 5);
    sw.sleep(20);
    auto dt2 = sw.getMs();
    TRACE("dt2: ", dt2);
    EXPECT_TRUE(dt2 >= (5+20));
    sw.sleep(35);
    auto dt3 = sw.getMs();
    TRACE("dt3: ", dt3);
    EXPECT_TRUE(dt3 >= (5+20+35));
}

TEST_F(StopwatchTest, sw02_clear)
{
    Stopwatch sw;
    sw.start();
    sw.sleep(5);
    auto dt1 = sw.getMs();
    TRACE("dt1: ", dt1);
    EXPECT_TRUE(dt1 >= 5);
    sw.stop();

    sw.start();
    sw.sleep(5);
    auto dt2 = sw.getMs();
    TRACE("dt2: ", dt2);
    EXPECT_TRUE(dt2 >= 5 && dt2 < 10);
    sw.stop();

    sw.start();
    sw.sleep(5);
    auto dt3 = sw.getMs();
    TRACE("dt3: ", dt3);
    EXPECT_TRUE(dt3 >= 5 && dt3 < 10);
    sw.stop();
}
