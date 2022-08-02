/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/NodeOutput.h>
#include <aif/pipe/NodeInput.h>

namespace aif {

NodeOutput::NodeOutput(const std::string& id, const NodeType& type)
: NodeIO(id, type)
{
}

NodeOutput::~NodeOutput()
{
}


bool NodeOutput::verifyConnect(const std::shared_ptr<NodeInput>& input) const
{
    return m_type.isContain(input->getType());
}

} // end of namespace aif
