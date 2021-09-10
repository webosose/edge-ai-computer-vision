#include <aif/tools/ConfigReader.h>
#include <aif/log/Logger.h>

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>
#include <fstream>
#include <iostream>

namespace rj = rapidjson;

namespace aif {

ConfigReader::ConfigReader(const std::string& fileName)
{
    std::ifstream ifs(fileName);
    if (ifs.is_open()) {
        rj::IStreamWrapper isw(ifs);
        m_document.ParseStream(isw);
        ifs.close();
        std::cout << "read config file: " << fileName << std::endl;
    } else {
        std::cout << "non exist config file: " << fileName << std::endl;
    }
}

std::string ConfigReader::getOption(const std::string& optionName)
{
    if (!m_document.IsObject()) {
        std::cout << "document is not object " << std::endl;
        return "";
    }
    return m_document[optionName.c_str()].GetString();
}

} // end of namespace aif
