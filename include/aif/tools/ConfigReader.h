#ifndef AIF_CONFIG_READER_H
#define AIF_CONFIG_READER_H

#include <rapidjson/document.h>
#include <string>

namespace aif {

class ConfigReader
{
public:
    ConfigReader(const std::string& fileName);
    std::string getOption(const std::string& optionName);

private:
    rapidjson::Document m_document;
};

} // end of namespace aif

#endif // AIF_CONFIG_READER_H
