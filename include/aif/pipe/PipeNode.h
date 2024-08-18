/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AIF_PIPE_NODE_H
#define AIF_PIPE_NODE_H

#include <aif/pipe/NodeInput.h>
#include <aif/pipe/NodeOutput.h>
#include <aif/pipe/NodeOperation.h>
#include <aif/tools/PerformanceReporter.h>

namespace aif {

class PipeNode {
    public:
        PipeNode();
        virtual ~PipeNode();

        const std::string& getId() const;
        const std::shared_ptr<NodeInput>& getInput() const;
        const std::shared_ptr<NodeOutput>& getOutput() const;

        virtual bool build(const std::shared_ptr<NodeConfig>& config);
        virtual bool rebuildOperation(const std::shared_ptr<NodeConfig>& config);
        virtual bool run();

        bool setDescriptor(const std::shared_ptr<PipeDescriptor>& descriptor);
        bool moveDescriptor(std::shared_ptr<PipeNode>& node);

        bool verifyConnectPrev(const std::shared_ptr<PipeNode>& node) const;
        bool verifyConnectNext(const std::shared_ptr<PipeNode>& node) const;

    private:
        std::string m_id;
        std::shared_ptr<NodeInput> m_input;
        std::shared_ptr<NodeOutput> m_output;
        std::shared_ptr<NodeOperation> m_operation;
        std::shared_ptr<PerformanceRecorder> m_performance;
};

} // end of namespace aif

#endif // AIF_PIPE_NODE_H
