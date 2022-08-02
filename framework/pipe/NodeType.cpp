/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/NodeType.h>

namespace aif {

NodeType::NodeType()
: m_type(NONE)
{
}

NodeType::NodeType(int type)
: m_type(type)
{
}

NodeType::~NodeType()
{
}

bool NodeType::operator==(const NodeType& other) const
{
    return (getInt() == other.getInt());
}

bool NodeType::operator!=(const NodeType& other) const
{
    return (getInt() != other.getInt());
}

int NodeType::fromString(const std::string& type)
{
    if (strcasecmp("IMAGE", type.c_str()) == 0) {
        return NodeType::IMAGE;
    }
    else if (strcasecmp("INFERENCE", type.c_str()) == 0) {
        return NodeType::INFERENCE;
    }

    return NodeType::NONE;
}

bool NodeType::isContain(int type) const
{
    return (type == (m_type & type));
}

bool NodeType::isContain(const NodeType& type) const
{
    return (type.getInt() == (m_type & type.getInt()));
}

void NodeType::addType(int type)
{
    m_type |= type;
}

std::string NodeType::toString() const
{
    std::string typeStr;
    if (isContain(NodeType::IMAGE)) typeStr += "IMAGE";
    if (isContain(NodeType::INFERENCE)) {
        if (!typeStr.empty()) typeStr += "|";
        typeStr += "INFERENCE";
    }
    if (typeStr.empty()) typeStr = "NONE";

    return typeStr;
}

int NodeType::getInt() const
{
    return m_type;
}

} // end of namespace aif
