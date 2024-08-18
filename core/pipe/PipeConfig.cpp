/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/PipeConfig.h>
#include <aif/pipe/NodeOperationFactory.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

namespace aif {

// NodeOperationConfig
bool NodeOperationConfig::parse(const rj::Value& value)
{
    if (!value.HasMember("type")) {
        Loge("invalid operation config (no type)");
        return false;
    }
    if (!value.HasMember("config")) {
        Loge("invalid operation config (no config)");
        return false;
    }
    const char* type = value["type"].GetString();
    m_type = (type ? type : "");
    m_config = jsonObjectToString(value["config"]);
    return true;
}

// BridgeConfig
bool BridgeOperationConfig::parse(const rj::Value& value)
{
    if (!NodeOperationConfig::parse(value)) {
        return false;
    }

    if (value["config"].HasMember("targetId")) {
        const char* target = value["config"]["targetId"].GetString();
        m_target = (target ? target : "");
    }
    return true;
}

// DetectorConfig
bool DetectorOperationConfig::parse(const rj::Value& value)
{
    if (!NodeOperationConfig::parse(value)) {
        return false;
    }
    if (!value.HasMember("config")) {
        Loge("invalid detector operation config (no config)");
        return false;
    }

    if (value["config"].HasMember("model")) {
        const char* model = value["config"]["model"].GetString();
        m_model = (model ? model : "");
    } else {
        Loge("invalid node config (no model)");
        return false;
    }

    if (value["config"].HasMember("param")) {
        m_param = jsonObjectToString(value["config"]["param"]);
    }
    return true;
}

// NodeConfig
bool NodeConfig::parse(const rj::Value& value)
{
    if (!value.HasMember("id")) {
        Loge("invalid node config (no id)");
        return false;
    }
    if (!value.HasMember("input")) {
        Loge("invalid node config (no input)");
        return false;
    }
    if (!value.HasMember("output")) {
        Loge("invalid node config (no output)");
        return false;
    }
    if (!value.HasMember("operation")) {
        Loge("invalid node config (no operation)");
        return false;
    }

    if (value.HasMember("id")) {
        const char* id = value["id"].GetString();
        m_id = (id ? id : "");
    }

    const rj::Value& itypes = value["input"];
    for (const auto& type : itypes.GetArray()) {
        if (type.GetString() != nullptr) {
            m_inputType.addType(NodeType::fromString(type.GetString()));
        }
    }
    const rj::Value& otypes= value["output"];
    for (const auto& otype: otypes.GetArray()) {
        if (otype.GetString() != nullptr) {
            m_outputType.addType(NodeType::fromString(otype.GetString()));
        }
    }

    m_operation = NodeOperationFactory::get().createConfig(value["operation"]);
    return (m_operation != nullptr);
}

// PipeConfig
bool PipeConfig::parse(const std::string& config)
{
    rj::Document doc;
    doc.Parse(config.c_str());
    if (!doc.HasMember("name")) {
        Loge("invalid pipe config (no name)");
        return false;
    }

    if (!doc.HasMember("nodes") || doc["nodes"].Size() == 0) {
        Loge("invalid pipe config (no nodes)");
        return false;
    }

    if (doc.HasMember("descriptor")) {
        const char* descriptorId = doc["descriptor"].GetString();
        m_descriptorId = (descriptorId ? descriptorId : "");
    } else {
        m_descriptorId = "default_descriptor";
    }

    if (doc.HasMember("name")) {
        const char* name = doc["name"].GetString();
        m_name = (name ? name : "");
    }
    const rj::Value& nodes = doc["nodes"];
    if (!nodes.IsArray() || nodes.Size() == 0) {
        Loge("invalid pipe config (invalid nodes)");
        return false;
    }

    for (auto& node : nodes.GetArray()) {
        const std::shared_ptr<NodeConfig> nodeConfig = std::make_shared<NodeConfig>();
        if (!nodeConfig->parse(node)) {
            return false;
        }
        if (!m_nodes.empty() &&
                !m_nodes.back()->getOutputType().isContain(nodeConfig->getInputType())) {
            Loge("failed to connect ", m_nodes.back()->getId(),  " -> ", nodeConfig->getId());
            return false;
        }
        m_nodes.emplace_back(nodeConfig);
    }
    return true;
}

std::shared_ptr<NodeConfig> PipeConfig::getNode(int index) const
{
    if (0 <= index && index < m_nodes.size()) {
        return m_nodes[index];
    }
    return nullptr;
}

} // end of namespace aif
