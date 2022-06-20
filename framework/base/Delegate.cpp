/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/Delegate.h>

namespace aif {

Delegate::Delegate(const::std::string& name, const std::string& option)
    : m_name(name)
    , m_option(option)
{
    if (m_option.empty()) {
        m_option = "{}";
    }
}

Delegate::~Delegate()
{
}

const std::string& Delegate::getName() const
{
    return m_name;
}

}   // end of namespace aif
