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
    if (m_document[optionName.c_str()].IsArray()) {
        return jsonObjectToString(m_document[optionName.c_str()]);
    }
    if (m_document[optionName.c_str()].IsObject()) {
        return jsonObjectToString(m_document[optionName.c_str()]);
    }
    if (m_document[optionName.c_str()].IsString()) {
        return m_document[optionName.c_str()].GetString();
    }
    if (m_document[optionName.c_str()].IsInt()) {
        return std::to_string(m_document[optionName.c_str()].GetInt());
    }
    if (m_document[optionName.c_str()].IsBool()) {
        return std::to_string(m_document[optionName.c_str()].GetBool());
    }
    if (m_document[optionName.c_str()].IsDouble()) {
        return std::to_string(m_document[optionName.c_str()].GetDouble());
    }
    if (m_document[optionName.c_str()].IsUint()) {
        return std::to_string(m_document[optionName.c_str()].GetUint());
    }
    if (m_document[optionName.c_str()].IsInt64()) {
        return std::to_string(m_document[optionName.c_str()].GetInt64());
    }
    if (m_document[optionName.c_str()].IsUint64()) {
        return std::to_string(m_document[optionName.c_str()].GetUint64());
    }
    if (m_document[optionName.c_str()].IsFloat()) {
        return std::to_string(m_document[optionName.c_str()].GetFloat());
    }
    if (m_document[optionName.c_str()].IsNumber()) {
        return std::to_string(m_document[optionName.c_str()].GetDouble());
    }
    return "";
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

std::vector<std::string> ConfigReader::getOptionArray(const std::string& optionName) const
{
    std::vector<std::string> result;
    if (!m_document.IsObject()) {
        Loge("document is not object");
        return result;
    }
    if (!m_document.HasMember(optionName.c_str())) {
        return result;
    }
    const auto& value = m_document[optionName.c_str()];
    if (!value.IsArray()) {
        Loge(optionName, " is not array");
        return result;
    }
    for (int i = 0; i < value.Size(); i++) {
        result.push_back(value[i].GetString());
    }
    return result;
}


} // end of namespace aif
