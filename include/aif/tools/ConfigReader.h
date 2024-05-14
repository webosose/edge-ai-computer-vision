/*
 * Copyright (c) 2022-2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CONFIG_READER_H
#define AIF_CONFIG_READER_H

#include <rapidjson/document.h>
#include <string>
#include <vector>

namespace aif {

class ConfigReader
{
public:
    ConfigReader(const std::string& fileName,
                 const std::string& defaultConfig = "");

    std::string getOption(const std::string& optionName) const;
    std::string getOptionObject(const std::string& optionName) const;
    std::vector<std::string> getOptionArray(const std::string& optionName) const;

private:
    void loadFromString(const std::string& config);

public:
    rapidjson::Document m_document;

};

} // end of namespace aif

#endif // AIF_CONFIG_READER_H
