/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NODE_OUTPUT_H
#define AIF_NODE_OUTPUT_H

#include <aif/pipe/NodeIO.h>

namespace aif {

class NodeInput;

class NodeOutput : public NodeIO {
    public:
        NodeOutput(const std::string& id, const NodeType& type);
        virtual ~NodeOutput();

        virtual bool verifyConnect(const std::shared_ptr<NodeInput>& input) const;
};

} // end of namespace aif

#endif // AIF_NODE_OUTPUT_H
