/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NODE_INPUT_H
#define AIF_NODE_INPUT_H

#include <aif/pipe/NodeIO.h>

namespace aif {

class NodeOutput;

class NodeInput : public NodeIO {
    public:
        NodeInput(const std::string& id, const NodeType& type);
        virtual ~NodeInput();
};

} // end of namespace aif

#endif // AIF_NODE_INPUT_H
