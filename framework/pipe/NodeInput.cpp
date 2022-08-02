/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/NodeInput.h>
#include <aif/pipe/NodeOutput.h>

namespace aif {

NodeInput::NodeInput(const std::string& id, const NodeType& type)
: NodeIO(id, type)
{
}

NodeInput::~NodeInput()
{
}

} // end of namespace aif
