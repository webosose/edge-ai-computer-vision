/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/NodeOperationFactory.h>
#include <aif/base/AIVision.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>

namespace aif {

t_aif_status NodeOperationFactory::registerGenerator(
        const std::string& type,
        const NodeOperationGenerator& generator,
        const NodeOperationConfigGenerator& configGenerator)
{
    try {
        if (m_generators.find(type) != m_generators.end()) {
            throw std::runtime_error(type + " operation generator is already registered");
        }
        if (m_configGenerators.find(type) != m_configGenerators.end()) {
            throw std::runtime_error(type + " operation generator is already registered");
        }
     } catch (const std::exception& e) {
        Loge(__func__, e.what());
        return kAifError;
    } catch (...) {
        Loge("Unknown exception occured!!");
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

    try {
        std::string type = config->getType();
        if (m_generators.find(type) == m_generators.end()) {
            throw std::runtime_error(type + " operation generator is not registered");
        }

        Logi(id, ": create operation - type: ", type);
        std::shared_ptr<NodeOperation> operation = m_generators[type](id);
        if (!operation->init(config)) {
            Loge("failed to initialize operation: ", id);
        }

        return operation;
    } catch (const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return nullptr;
    } catch (...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return nullptr;
    }
}

std::shared_ptr<NodeOperationConfig> NodeOperationFactory::createConfig(const rj::Value& value)
{
    try {
        if (!value.HasMember("type")) {
            throw std::runtime_error("node operation config has no type");
        }
        std::string type = value["type"].GetString();
        if (m_configGenerators.find(type) == m_configGenerators.end())  {
            throw std::runtime_error(type + " operation config generator is not registered");
        }
        std::shared_ptr<NodeOperationConfig> config= m_configGenerators[type]();
        if (!config->parse(value)) {
            throw std::runtime_error("failed to parse node operation config " + type);
        }
        return config;
    } catch (const std::exception& e) {
        Loge(__func__,"Error: ", e.what());
        return nullptr;
    } catch (...) {
        Loge(__func__,"Error: Unknown exception occured!!");
        return nullptr;
    }
}

} // end of namespace aif
