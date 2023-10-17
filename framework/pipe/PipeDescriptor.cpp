/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/PipeDescriptor.h>
#include <aif/pipe/NodeType.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace aif {

PipeDescriptor::PipeDescriptor()
: m_type(NodeType::NONE)
, m_startTime(0)
{
    m_root.SetObject();
}

PipeDescriptor::~PipeDescriptor()
{
}

void PipeDescriptor::setStartTimestamp(int64_t timestamp)
{
    m_startTime = timestamp;
}

void PipeDescriptor::setImage(const cv::Mat& image)
{
    m_type.addType(NodeType::IMAGE);
    m_image = image;
}

const NodeType& PipeDescriptor::getType() const
{
    return m_type;
}

const cv::Mat& PipeDescriptor::getImage() const
{
    if (!m_type.isContain(NodeType::IMAGE)) {
        Loge("failed to getImage - descriptor type : ", m_type.toString());
    }

    return m_image;
}

std::string PipeDescriptor::getResult() const
{
    if (!m_type.isContain(NodeType::INFERENCE)) {
        Loge("failed to getResult- descriptor type : ", m_type.toString());
        return "";
    }

    rj::StringBuffer buffer;
    rj::Writer<rj::StringBuffer> writer(buffer);
    writer.SetMaxDecimalPlaces(4);
    m_root.Accept(writer);
    return buffer.GetString();
}

std::string PipeDescriptor::getResult(const std::string& nodeId) const
{
    if (!m_type.isContain(NodeType::INFERENCE)) {
        Loge("failed to getResult- descriptor type : ", m_type.toString());
        return "";
    }

    for (auto& result : m_root["results"].GetArray()) {
        if (result.HasMember(nodeId)) {
            return jsonObjectToString(result[nodeId]);
        }
    }
    return "";
}

bool PipeDescriptor::addBridgeOperationResult(
        const std::string& nodeId,
        const std::string& operationType,
        const std::string& result)
{
    Logd(nodeId, ": add bridge operation result");
    Logd(operationType, " : ",  result);

    return addStringResult(nodeId, result);
}

bool PipeDescriptor::addDetectorOperationResult(
        const std::string& nodeId,
        const std::string& model,
        const std::shared_ptr<Descriptor>& descriptor)
{
    std::string result = descriptor->toStr();
    Logd(nodeId, ": add detector operation result - ", result);
    Logd(model, " : ",  result);

    return addStringResult(nodeId, result);
}

bool PipeDescriptor::addStringResult(
        const std::string& nodeId, const std::string& result)
{
    m_type.addType(NodeType::INFERENCE);
    rj::Document document;
    document.Parse(result.c_str());

    rj::Document::AllocatorType& allocator = m_root.GetAllocator();
    if (!m_root.HasMember("results")) {
        rj::Value results(rj::kArrayType);
        m_root.AddMember("results", results, allocator);
    }

    rj::Value res(rj::kObjectType);
    res.AddMember(
            rj::Value(nodeId.c_str(), allocator),
            rj::Value(document, allocator),
            allocator);

    m_root["results"].PushBack(res, allocator);
    return true;
}

const ExtraOutputs& PipeDescriptor::getExtraOutputs() const
{
    return m_extraOutputs;
}

bool PipeDescriptor::getExtraOutput(const std::string& nodeId, ExtraOutput& extraOutput)
{
    if (m_extraOutputs.find(nodeId) == m_extraOutputs.end()) {
        return false;
    }
    extraOutput = m_extraOutputs[nodeId];
    return true;
}

void PipeDescriptor::initExtraOutputs(const ExtraOutputs& extraOutputs)
{
    m_extraOutputs = extraOutputs;
}

} // end of namespace aif
