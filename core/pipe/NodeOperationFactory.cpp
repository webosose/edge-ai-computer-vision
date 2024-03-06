/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/NodeOperationFactory.h>
#include <aif/base/AIVision.h>
#include <aif/base/ExtensionLoader.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

namespace aif {

t_aif_status NodeOperationFactory::registerGenerator(
        const std::string& type,
        const NodeOperationGenerator& generator,
        const NodeOperationConfigGenerator& configGenerator)
{
    if (m_generators.find(type) != m_generators.end()) {
        Loge(type, " operation generator is already registered");
        return kAifError;
    }
    if (m_configGenerators.find(type) != m_configGenerators.end()) {
        Loge(type, " operation generator is already registered");
        return kAifError;
    }

    m_generators[type] = generator;
    m_configGenerators[type] = configGenerator;
    return kAifOk;
}

std::shared_ptr<NodeOperation> NodeOperationFactory::create(
        const std::string& id,
        const std::shared_ptr<NodeOperationConfig>& config)
{
    if (!AIVision::isInitialized()) {
        Loge(__func__, "AIVision is not initialized");
        return nullptr;
    }

    std::string type = config->getType();
    if (m_generators.find(type) == m_generators.end()) {
        if (ExtensionLoader::get().isAvailable(type, kNodeOperation) != kAifOk) {
            Loge(type, " operation generator is not registered, ", type);
            return nullptr;
        }
        if (ExtensionLoader::get().enableFeature(type, kNodeOperation) != kAifOk) {
            Loge(type, " ExtensionLoader enableFeature error");
            return nullptr;
        }
    }

    Logi(id, ": create operation - type: ", type);
    std::shared_ptr<NodeOperation> operation = m_generators[type](id);
    if (!operation->init(config)) {
        Loge("failed to initialize operation: ", id);
        return nullptr;
    }

    return operation;
}

std::shared_ptr<NodeOperationConfig> NodeOperationFactory::createConfig(const rj::Value& value)
{
    if (!value.HasMember("type")) {
        Loge("node operation config has no type");
        return nullptr;
    }
    std::string type = value["type"].GetString();
    if (m_configGenerators.find(type) == m_configGenerators.end())  {
        if (ExtensionLoader::get().isAvailable(type, kNodeOperation) != kAifOk) {
            Loge(type, " operation config generator is not registered");
            return nullptr;
        }
        if (ExtensionLoader::get().enableFeature(type, kNodeOperation) != kAifOk) {
            Loge(type, " ExtensionLoader enableFeature error");
            return nullptr;
        }
    }
    std::shared_ptr<NodeOperationConfig> config= m_configGenerators[type]();
    if (!config->parse(value)) {
        Loge("failed to parse node operation config ", type);
        return nullptr;
    }
    return config;
}

void NodeOperationFactory::clear()
{
    m_generators.clear();
    m_configGenerators.clear();
}

} // end of namespace aif
