/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NODE_OPERATION_FACTORY_H
#define AIF_NODE_OPERATION_FACTORY_H

#include <aif/base/Types.h>
#include <aif/pipe/NodeOperation.h>

#include <string>
#include <unordered_map>

namespace aif {

using NodeOperationGenerator =
    std::shared_ptr<NodeOperation>(*)(const::std::string& id);

using NodeOperationConfigGenerator =
    std::shared_ptr<NodeOperationConfig>(*)();

class NodeOperationFactory
{
public:
    static NodeOperationFactory& get() {
        static NodeOperationFactory instance;
        return instance;
    }

    t_aif_status registerGenerator(
            const std::string& type,
            const NodeOperationGenerator& generator,
            const NodeOperationConfigGenerator& configGenerator);

    std::shared_ptr<NodeOperation> create(
            const std::string& id,
            const std::shared_ptr<NodeOperationConfig>& config);

    std::shared_ptr<NodeOperationConfig> createConfig(
            const rj::Value& value);

    std::vector<std::string> getOperationTypes() const {
        std::vector<std::string> types;
        for (const auto& kv : m_configGenerators) {
            types.push_back(kv.first);
        }
        return types;
    }
    
    void clear();

private:
    NodeOperationFactory() {}
    NodeOperationFactory(const NodeOperationFactory&) {}
    ~NodeOperationFactory() {}

private:
    std::unordered_map<std::string, NodeOperationGenerator> m_generators;
    std::unordered_map<std::string, NodeOperationConfigGenerator> m_configGenerators;
};

} // end of namespace aif

#endif  // AIF_NODE_OPERATION_FACTORY_H
