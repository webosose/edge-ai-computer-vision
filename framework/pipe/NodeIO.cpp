/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/pipe/NodeIO.h>
#include <aif/pipe/NodeType.h>
#include <aif/log/Logger.h>

namespace aif {

NodeIO::NodeIO(const std::string& id, const NodeType& type)
 : m_id(id), m_type(type)
{
}

NodeIO::~NodeIO()
{
}

const std::shared_ptr<NodeDescriptor>& NodeIO::getDescriptor() const
{
    if (!m_descriptor) {
        Loge(m_id, ": node descriptor is null");
        return m_descriptor;
    }
    if (!verifyDescriptor(m_descriptor)) {
        Loge(m_id, ": failed to get node descriptor(invalid descriptor)");
        Loge("io type : ", m_type.toString());
        Loge("descriptor type: ", m_descriptor->getType().toString());
    }
    return m_descriptor;
}

bool NodeIO::setDescriptor(const std::shared_ptr<NodeDescriptor>& descriptor)
{
    if (!verifyDescriptor(descriptor)) {
        Loge(m_id, ": failed to set node descriptor(invalid descriptor)");
        Loge("io type : ", m_type.toString());
        Loge("descriptor type: ", descriptor->getType().toString());
        return false;
    }

    m_descriptor = descriptor;
    return true;
}

const NodeType& NodeIO::getType() const
{
    return m_type;
}

const std::string& NodeIO::getId() const
{
    return m_id;
}

bool NodeIO::verifyDescriptor(const std::shared_ptr<NodeDescriptor>& descriptor) const
{
    return descriptor->getType().isContain(m_type);
}

}
