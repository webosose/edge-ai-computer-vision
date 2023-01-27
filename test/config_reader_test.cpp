/*
 * Copyright (c) 2022-2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/tools/ConfigReader.h>
#include <aif/log/Logger.h>

#include <gtest/gtest.h>

using namespace std;
using namespace aif;

class ConfigReaderTest: public ::testing::Test
{
protected:
    ConfigReaderTest() = default;
    ~ConfigReaderTest() = default;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(ConfigReaderTest, 01_getOption)
{
    string option =
        "{ " \
            "\"LogLevel\" : \"WARNING\"," \
            "\"BasePath\" : \"/usr/share/test\"" \
        "}";
    ConfigReader cr("", option);
    EXPECT_EQ(cr.getOption("LogLevel"), "WARNING");
    EXPECT_EQ(cr.getOption("BasePath"), "/usr/share/test");
}

TEST_F(ConfigReaderTest, 02_getOptionArray)
{
    string option = R"(
        {
            "LogLevel" : "WARNING",
            "BasePath" : "/usr/share/test",
            "ReportType" : ["CONSOLE", "REPORT"]
        })";

    ConfigReader cr("", option);
    EXPECT_EQ(cr.getOptionArray("LogLevel").size(), 0);
    std::vector<std::string> result = cr.getOptionArray("ReportType");
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], "CONSOLE");
    EXPECT_EQ(result[1], "REPORT");

}
