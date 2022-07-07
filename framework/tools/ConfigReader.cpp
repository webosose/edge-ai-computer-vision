/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/tools/ConfigReader.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>
#include <fstream>
#include <iostream>


namespace rj = rapidjson;

namespace aif {

ConfigReader::ConfigReader(const std::string& fileName, const std::string& defaultConfig)
{
    if (!fileName.empty()) {
        std::ifstream ifs(fileName);
        if (ifs.is_open()) {
            rj::IStreamWrapper isw(ifs);
            m_document.ParseStream(isw);
            ifs.close();
            std::cout << "Load config file: " << fileName << std::endl;
        }
    } else if (!defaultConfig.empty()){
        loadFromString(defaultConfig);
    }
}

void ConfigReader::loadFromString(const std::string& config)
{
    rj::ParseResult res = m_document.Parse(config.c_str());
    if (!res) {
        std::cout << "Failed to load config string: " << std::endl << config << std::endl;
        return;
    }
    std::cout << "Load default config: " << config << std::endl;
}

std::string ConfigReader::getOption(const std::string& optionName) const
{
    if (!m_document.IsObject()) {
        Loge("document is not object");
        return "";
    }
    if (!m_document.HasMember(optionName.c_str())) {
        return "";
    }
    return m_document[optionName.c_str()].GetString();
}

std::string ConfigReader::getOptionObject(const std::string& optionName) const
{
    std::string option;
    if (!m_document.IsObject()) {
        Loge("document is not object");
        return "";
    }
    if (!m_document.HasMember(optionName.c_str())) {
        return "";
    }

    return jsonObjectToString(m_document[optionName.c_str()]);
}

} // end of namespace aif
