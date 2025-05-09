/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_NODE_IO_H
#define AIF_NODE_IO_H

#include <aif/pipe/NodeType.h>
#include <aif/pipe/PipeDescriptor.h>

#include <string>
#include <memory>

namespace aif {

class NodeIO {
    public:
        NodeIO(const std::string& id, const NodeType& type);
        virtual ~NodeIO();

        const std::shared_ptr<PipeDescriptor>& getDescriptor() const;
        bool setDescriptor(const std::shared_ptr<PipeDescriptor>& descriptor);
        const NodeType& getType() const;
        const std::string& getId() const;

    protected:
        bool verifyDescriptor(const std::shared_ptr<PipeDescriptor>& descritor) const;

    protected:
        std::string m_id;
        NodeType m_type;
        std::shared_ptr<PipeDescriptor> m_descriptor;

};

} // end of namespace aif

#endif // AIF_NODE_IO_H
