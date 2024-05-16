/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/log/Logger.h>
#include <aif/tools/MakeConfig.h>

using json = nlohmann::json;

namespace aif {

std::string makeDetectConfig(const std::string &model, json &param)
{
    return (param.is_null()) ? json {{"model", model}}.dump(4) :
                               json{{"model", model},
                                    {"param", param}}.dump(4); // for Debug! change it TODO
}


json createPipeNode(const std::string &id, const std::string &op_type,
                    json input, json output, const json &op_config)
{
    json node;
    node["id"] = id;
    node["input"] = std::move(input);
    node["output"] = std::move(output);
    node["operation"]["type"] = op_type;
    node["operation"]["config"] = (op_config.is_null()) ? R"({})"_json : op_config;

    return node;
}

} // end of namespace aif
