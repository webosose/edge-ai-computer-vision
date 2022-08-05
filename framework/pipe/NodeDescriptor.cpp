/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/NodeDescriptor.h>
#include <aif/pipe/NodeType.h>
#include <aif/tools/Utils.h>
#include <aif/log/Logger.h>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace aif {

NodeDescriptor::NodeDescriptor()
: m_type(NodeType::NONE)
{
    m_result.SetObject();
}

NodeDescriptor::~NodeDescriptor()
{
}

void NodeDescriptor::setImage(const cv::Mat& image)
{
    m_type.addType(NodeType::IMAGE);
    m_image = image;
}

void NodeDescriptor::addOperationResult(const std::string& id, const std::string& result)
{
    Logi(id, ": add operation result - ", result);
    m_type.addType(NodeType::INFERENCE);
    rj::Document document;
    document.Parse(result.c_str());

    rj::Document::AllocatorType& allocator = m_result.GetAllocator();
    if (!m_result.HasMember("results")) {
        rj::Value results(rj::kArrayType);
        m_result.AddMember("results", results, allocator);
    }

    rj::Value res(rj::kObjectType);
    res.AddMember(
            rj::Value(id.c_str(), allocator),
            rj::Value(document, allocator),
            allocator);

    m_result["results"].PushBack(res, allocator);
}

const NodeType& NodeDescriptor::getType() const
{
    return m_type;
}

const cv::Mat& NodeDescriptor::getImage() const
{
    if (!m_type.isContain(NodeType::IMAGE)) {
        Loge("failed to getImage - descriptor type : ", m_type.toString());
    }

    return m_image;
}

std::string NodeDescriptor::getResult() const
{
    if (!m_type.isContain(NodeType::INFERENCE)) {
        Loge("failed to getResult- descriptor type : ", m_type.toString());
        return "";
    }

    rj::StringBuffer buffer;
    rj::Writer<rj::StringBuffer> writer(buffer);
    writer.SetMaxDecimalPlaces(4);
    m_result.Accept(writer);
    return buffer.GetString();
}

std::string NodeDescriptor::getResult(const std::string& id) const
{
    if (!m_type.isContain(NodeType::INFERENCE)) {
        Loge("failed to getResult- descriptor type : ", m_type.toString());
        return "";
    }

    for (auto& result : m_result["results"].GetArray()) {
        if (result.HasMember(id)) {
            return jsonObjectToString(result[id]);
        }
    }
    return "";
}

} // end of namespace aif
