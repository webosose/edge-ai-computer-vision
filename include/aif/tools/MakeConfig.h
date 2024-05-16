/*
 * Copyright (c) 2024 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_MAKECONFIG_H
#define AIF_MAKECONFIG_H

#include <string>
#include <nlohmann/json.hpp>

using namespace nlohmann;
namespace aif {


std::string makeDetectConfig(const std::string &model, json &param);

json createPipeNode(const std::string &id, const std::string &op_type,
                    json input, json output, const json &op_config);

} // end of namespace aif
#endif // AIF_MAKECONFIG_H
