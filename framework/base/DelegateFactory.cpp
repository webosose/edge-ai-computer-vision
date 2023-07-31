/*
 * Copyright (c) 2022 LG Electronics Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aif/base/DelegateFactory.h>
#include <aif/base/AIVision.h>
#include <aif/log/Logger.h>

namespace aif {

t_aif_status DelegateFactory::registerGenerator(
        const std::string& id,
        const DelegateGenerator& delegateGenerator)
{
    if (m_delegateGenerators.find(id) != m_delegateGenerators.end()) {
        Loge(id, " delegate generator is already registered");
        return kAifError;
    }

    m_delegateGenerators[id] = delegateGenerator;
    return kAifOk;
}

std::shared_ptr<Delegate> DelegateFactory::getDelegate(const std::string& id, const std::string& option)
{
    if (!AIVision::isInitialized()) {
        Loge(__func__, "AIVision is not initialized");
        return nullptr;
    }
    if (m_delegates.find(id) == m_delegates.end() &&
            m_delegateGenerators.find(id) == m_delegateGenerators.end()) {
        Loge(id, " delegate generator is not registered");
        return nullptr;
    }
    if (m_delegates.find(id) != m_delegates.end()) {
        return m_delegates[id];
    }
    m_delegates[id] = m_delegateGenerators[id](option);
    return m_delegates[id];
}

} // end of idspace aif
