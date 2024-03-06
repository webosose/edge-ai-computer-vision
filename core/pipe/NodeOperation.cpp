/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/NodeOperation.h>
#include <aif/log/Logger.h>

namespace aif {

NodeOperation::NodeOperation(const std::string& id)
: m_id(id)
{
}

NodeOperation::~NodeOperation()
{
}

bool NodeOperation::init(const std::shared_ptr<NodeOperationConfig>& config)
{
    m_config = config;
    return (m_config != nullptr);
}

bool NodeOperation::run(
        const std::shared_ptr<NodeInput>& input,
        const std::shared_ptr<NodeOutput>& output)
{
    if (!runImpl(input)) {
        Loge(m_id, " op: failed to do operation: ", m_id);
        return false;
    }
    return output->setDescriptor(input->getDescriptor());
}


const std::string& NodeOperation::getId() const
{
    return m_id;
}

const std::shared_ptr<NodeOperationConfig>& NodeOperation::getConfig() const
{
    return m_config;
}

} // end of namespace aif
