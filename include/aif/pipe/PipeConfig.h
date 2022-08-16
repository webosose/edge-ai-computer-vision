/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_PIPE_CONFIG_H
#define AIF_PIPE_CONFIG_H

#include <vector>

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <aif/pipe/NodeType.h>
#include <rapidjson/document.h>

#include <memory>

namespace rj = rapidjson;
namespace aif {

class BaseConfig {
    public:
        BaseConfig() {}
        virtual ~BaseConfig() {}
        virtual bool parse(const rj::Value& value) = 0;
};

class NodeOperationConfig : public BaseConfig {
    public:
        NodeOperationConfig() : BaseConfig() {}
        virtual ~NodeOperationConfig() {}

        bool parse(const rj::Value& value) override;
        const std::string& getType() const { return m_type; }
        const std::string& getConfig() const { return m_config; }

    protected:
        std::string m_type;
        std::string m_config;
};

class BridgeOperationConfig : public NodeOperationConfig {
    public:
        BridgeOperationConfig() : NodeOperationConfig() {}
        virtual ~BridgeOperationConfig() {}

        bool parse(const rj::Value& value) override;
        const std::string& getTargetId() const { return m_target; }

    protected:
        std::string m_target;
};

class DetectorOperationConfig : public NodeOperationConfig {
    public:
        DetectorOperationConfig() : NodeOperationConfig() {}
        virtual ~DetectorOperationConfig() {}

        bool parse(const rj::Value& value) override;
        const std::string& getModel() const { return m_model; }
        const std::string& getParam() const { return m_param; }

    protected:
        std::string m_model;
        std::string m_param;
};

class NodeConfig : public BaseConfig {
    public:
        NodeConfig() : BaseConfig() {}
        virtual ~NodeConfig() {}

        bool parse(const rj::Value& value) override;

        const std::string& getId() const { return m_id; }
        const NodeType& getInputType() const { return m_inputType; }
        const NodeType& getOutputType() const { return m_outputType; }
        const std::shared_ptr<NodeOperationConfig>& getOperation() const { return m_operation; }

    private:
        std::string m_id;
        NodeType m_inputType;
        NodeType m_outputType;
        std::shared_ptr<NodeOperationConfig> m_operation;
};

class PipeConfig {
    public:
        PipeConfig() {}
        ~PipeConfig() {}

        bool parse(const std::string& config);

        const std::string& getName() const { return m_name; }
        const std::string& getDescriptorId() const { return m_descriptorId; }
        size_t getNodeSize() const { return m_nodes.size(); }
        std::shared_ptr<NodeConfig> getNode(int index) const;

    private:
        std::string m_name;
        std::string m_descriptorId;
        std::vector<std::shared_ptr<NodeConfig>> m_nodes;
};

} // end of namespace aif

#endif // AIF_PIPE_CONFIG_H
