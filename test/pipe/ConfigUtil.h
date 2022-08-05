/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_CONFIG_UTIL_H
#define AIF_CONFIG_UTIL_H

#include <aif/pipe/PipeConfig.h>

namespace aif {

class ConfigUtil {
public:
    ConfigUtil() {}
    ~ConfigUtil() {}

    static rj::Value stringToJson(const std::string& config)
    {
        rj::Document doc;
        doc.Parse(config.c_str());
        rj::Value temp(doc, doc.GetAllocator());
        return temp;
    }

    template <typename T>
    static std::shared_ptr<NodeOperationConfig> getOperationConfig(const std::string& config)
    {
        std::shared_ptr<NodeOperationConfig> noc = std::make_shared<T>();
        noc->parse(stringToJson(config));
        return noc;
    }

    template <typename T>
    static std::shared_ptr<T> getConfig(const std::string& config)
    {
        std::shared_ptr<T> nc = std::make_shared<T>();
        nc->parse(stringToJson(config));
        return nc;
    }
};

} // end of namespace aif

#endif // AIF_CONFIG_UTIL_H
