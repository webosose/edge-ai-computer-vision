/*
 * Copyright (c) 2023 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/rppg/RppgInferencePipeDescriptor.h>
#include <aif/rppg/RppgInferencePipeDescriptorRegistration.h>
#include <aif/pipe/NodeType.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace aif {

RppgInferencePipeDescriptor::RppgInferencePipeDescriptor()
: PipeDescriptor()
, m_rppgOutputs()
, m_batchSize(200)
, m_channelSize(1)
, m_bpm(0)
, m_signalCondition("Bad")
{
}

RppgInferencePipeDescriptor::~RppgInferencePipeDescriptor()
{
}

bool RppgInferencePipeDescriptor::addBridgeOperationResult(
        const std::string& nodeId,
        const std::string& operationType,
        const std::string& result)
{
    if (operationType.rfind("rppg_post_process", 0) == 0) {
        if(addRppgFinalResult(m_bpm, m_signalCondition)) return true;
    }
    return false;
}

bool RppgInferencePipeDescriptor::addDetectorOperationResult(
        const std::string& nodeId,
        const std::string& model,
        const std::shared_ptr<Descriptor>& descriptor)
{
    if (model.rfind("rppg", 0) == 0) {
        auto rppg = std::dynamic_pointer_cast<RppgDescriptor>(descriptor);
        return appendRppg(nodeId, rppg);
    }
    return false;
}


bool RppgInferencePipeDescriptor::appendRppg(
        const std::string& nodeId,
        const std::shared_ptr<RppgDescriptor> descriptor)
{
    m_type.addType(NodeType::INFERENCE);
    if(descriptor->getRppgSize() == 0) {
        Logi("Failed to appendRppgDectectorResult");
        return false;
    }
    else {
        const auto& rppgs = descriptor->getRppg();
        m_rppgOutputs.insert(m_rppgOutputs.end(), rppgs.begin(), rppgs.end());
    }
    return true;
}

bool RppgInferencePipeDescriptor::addRppgFinalResult(float bpm, std::string signalCondition)
{
    rj::Document::AllocatorType &allocator = m_root.GetAllocator();
    if (!m_root.HasMember("rPPG")) {
        rj::Value rPPG(rj::kArrayType);
        m_root.AddMember("rPPG", rPPG, allocator);
    }
    rj::Value data(rj::kObjectType);
    // add signal condition and bpm
    data.AddMember("signalCondition", signalCondition, allocator);
    data.AddMember("bpm", bpm, allocator);
    m_root["rPPG"].PushBack(data, allocator);

    return true;
}

} // end of namespace aif
