/*
 * Copyright (c) 2022-2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/AIVision.h>
#include <aif/tools/ConfigReader.h>
#include <aif/log/Logger.h>

#include <boost/filesystem.hpp>
#include <rapidjson/istreamwrapper.h>
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

// This test will always sucess, the purpose is to print all key and value of mainConfigPath
TEST_F(ConfigReaderTest, 03_MainConfigTest)
{
    std::string mainConfigPath = std::string(EDGEAI_VISION_HOME) + "/" + std::string(EDGEAI_VISION_CONFIG);
    if (!boost::filesystem::exists(mainConfigPath))
    {
        std::cout << "Main config file does not exist: " << mainConfigPath << std::endl;
        SUCCEED();
        return;
    }
    ConfigReader mainConfig(mainConfigPath);
    //print all key and value of mainConfigPath
    std::cout << "Config in "<< mainConfigPath << std::endl;
    for (auto& member : mainConfig.m_document.GetObject()) {
        if (member.name.IsString()) {
            std::cout << member.name.GetString() << ": " << mainConfig.getOption(member.name.GetString()) << std::endl;
        }
    }
    SUCCEED();
}

// This test will always sucess, the purpose is to print all key and value of subConfigPath
TEST_F(ConfigReaderTest, 04_SubConfigTest)
{
    std::string subConfigPath = std::string(EDGEAI_VISION_HOME_SUB) + "/" + std::string(EDGEAI_VISION_CONFIG);
    // if subConfigPath does not exist, the test will success
    if (!boost::filesystem::exists(subConfigPath))
    {
        std::cout << "Sub config file does not exist: " << subConfigPath << std::endl;
        SUCCEED();
        return;
    }
    ConfigReader subConfig(subConfigPath);
    //print all key and value of subConfigPath
    std::cout << "Config in "<< subConfigPath << std::endl;
    for (auto& member : subConfig.m_document.GetObject()) {
        if (member.name.IsString()) {
            std::cout << member.name.GetString() << ": " << subConfig.getOption(member.name.GetString()) << std::endl;
        }
    }
    SUCCEED();
}

// This is for testing mergeConfig function
TEST_F(ConfigReaderTest, 05_MergeConfigTest)
{
    std::string mainConfigPath = std::string(EDGEAI_VISION_HOME) + "/" + std::string(EDGEAI_VISION_CONFIG);
    std::string subConfigPath = std::string(EDGEAI_VISION_HOME_SUB) + "/" + std::string(EDGEAI_VISION_CONFIG);
    // if one of config file does not exist, the test will success
    if (!boost::filesystem::exists(mainConfigPath) || !boost::filesystem::exists(subConfigPath))
    {
        std::cout << "One of Config files does not exist: " << subConfigPath << std::endl;
        SUCCEED();
        return;
    }

    auto config = std::make_unique<ConfigReader>(mainConfigPath);
    auto override_config = std::make_unique<ConfigReader>(subConfigPath);
    AIVision::mergeConfig(config, override_config);
    for (auto& member : override_config->m_document.GetObject()) {
        if (member.name.IsString()) {
            EXPECT_EQ(config->m_document.HasMember(member.name), true);
            EXPECT_EQ(config->getOption(member.name.GetString()), override_config->getOption(member.name.GetString()));
        }
    }
    //print all key and value of config
    std::cout << "Merged Config" << std::endl;
    for (auto& member : config->m_document.GetObject()) {
        if (member.name.IsString()) {
            std::cout << member.name.GetString() << ": " << config->getOption(member.name.GetString()) << std::endl;
        }
    }
    SUCCEED();
}

