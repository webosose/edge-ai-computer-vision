/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NODE_OPERATION_H
#define AIF_NODE_OPERATION_H

#include <aif/pipe/PipeConfig.h>
#include <aif/pipe/NodeInput.h>
#include <aif/pipe/NodeOutput.h>

namespace aif {

class NodeOperation {
    protected:
       NodeOperation(const std::string& id);
       virtual ~NodeOperation();

    public:
       virtual bool init(const std::shared_ptr<NodeOperationConfig>& config);
       virtual bool update(const std::shared_ptr<NodeOperationConfig>& config) = 0;
       bool run(const std::shared_ptr<NodeInput>& input,
               const std::shared_ptr<NodeOutput>& output);

       const std::string& getId() const;
       const std::shared_ptr<NodeOperationConfig>& getConfig() const;

    protected:
       virtual bool runImpl(const std::shared_ptr<NodeInput>& input) = 0;

    protected:
       std::string m_id;
       std::shared_ptr<NodeOperationConfig> m_config;
};

} // end of namespace aif

#endif // AIF_NODE_OPERATION_H
