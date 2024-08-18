/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */
#include <aif/pipe/PipeNode.h>
#include <aif/pipe/NodeOperationFactory.h>
#include <aif/log/Logger.h>
#include <aif/tools/Utils.h>
#include <aif/tools/Stopwatch.h>

namespace aif {

PipeNode::PipeNode()
{
}

PipeNode::~PipeNode()
{
}

const std::string& PipeNode::getId() const
{
    return m_id;
}

const std::shared_ptr<NodeInput>& PipeNode::getInput() const
{
    return m_input;
}

const std::shared_ptr<NodeOutput>& PipeNode::getOutput() const
{
    return m_output;
}

bool PipeNode::build(const std::shared_ptr<NodeConfig>& config)
{
    Logi(config->getId(), ": build pipe node");
    m_id = config->getId();
    m_input = std::make_shared<NodeInput>(m_id, config->getInputType());
    m_output = std::make_shared<NodeOutput>(m_id, config->getOutputType());
    m_operation = NodeOperationFactory::get().create(m_id, config->getOperation());
    m_performance = std::make_shared<PerformanceRecorder>(m_id, "", Performance::TargetType::PIPENODE);
    PerformanceReporter::get().addRecorder(m_id, m_performance);

    if (!m_input || !m_output || !m_operation || !m_performance) {
        Loge(m_id, ": failed to build pipe node");
        return false;
    }
    return true;
}

bool PipeNode::rebuildOperation(const std::shared_ptr<NodeConfig>& config)
{
    Logi(config->getId(), ": build pipe node operation");

    if (m_id != config->getId()) {
        Loge(" failed: newConfig->getId is different! ", m_id, " != ", config->getId());
        return false;
    }
    if (m_input->getType() != config->getInputType()) {
        Loge(" failed: newConfig->getInputType is different! ", m_input->getType().toString(), " != ", config->getInputType().toString());
        return false;
    }
    if (m_output->getType() != config->getOutputType()) {
        Loge(" failed: newConfig->getOutputType is different! ", m_output->getType().toString(), " != ", config->getOutputType().toString());
        return false;
    }

    auto origOpConfig = m_operation->getConfig();
    auto newOpConfig = config->getOperation();
    if (origOpConfig->getConfig() != newOpConfig->getConfig()) {
        return m_operation->update(newOpConfig);
    }

    return true;
}

bool PipeNode::run()
{
    Stopwatch sw;
    sw.start();
    m_performance->start(Performance::NODEPROCESS);
    if (!m_operation->run(m_input, m_output)) {
        Loge(m_id, ": failed to do operation");
        return false;
    }
    if (!m_output->setDescriptor(m_input->getDescriptor())) {
        Loge(m_id, ": failed to set node descriptor into NodeOutput");
        return false;
    }
    m_performance->stop(Performance::NODEPROCESS);
    Logd(m_id, ": node detect time : ", sw.getMs(), "ms");
    sw.stop();

    return true;
}

bool PipeNode::setDescriptor(const std::shared_ptr<PipeDescriptor>& descriptor)
{
    if (!m_input) {
        Loge(m_id, ": input is null");
        return false;
    }
    return m_input->setDescriptor(descriptor);
}

bool PipeNode::moveDescriptor(std::shared_ptr<PipeNode>& node)
{
    if (!node) {
        Loge(m_id, ": node is null");
        return false;
    }
    if (!m_output->getDescriptor()) {
        return false;
    }
    Logd(m_id, ": move descriptor ", getId(), " -> ", node->getId());
    return node->setDescriptor(m_output->getDescriptor());
}

bool PipeNode::verifyConnectPrev(const std::shared_ptr<PipeNode>& node) const
{
    if (!m_input) {
        Loge(m_id, ": input is null");
        return false;
    }
    return node->getOutput()->verifyConnect(getInput());

}

bool PipeNode::verifyConnectNext(const std::shared_ptr<PipeNode>& node) const
{
    if (!m_output) {
        Loge(m_id, ": output is null");
        return false;
    }
    return m_output->verifyConnect(node->getInput());
}

} // end of namespace aif
